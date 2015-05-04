package gradlecpp

import gradlecpp.teamcity.TeamCityIntegration
import org.gradle.api.Action
import org.gradle.api.GradleException
import org.gradle.api.Plugin
import org.gradle.api.Project
import org.gradle.api.Task
import org.gradle.api.internal.project.AbstractProject
import org.gradle.model.internal.core.DirectNodeModelAction
import org.gradle.model.internal.core.ModelActionRole
import org.gradle.model.internal.core.ModelPath
import org.gradle.model.internal.core.ModelReference
import org.gradle.model.internal.core.MutableModelNode
import org.gradle.model.internal.core.rule.describe.ModelRuleDescriptor
import org.gradle.model.internal.core.rule.describe.SimpleModelRuleDescriptor
import org.gradle.model.internal.registry.ModelRegistry
import org.gradle.nativeplatform.NativeBinarySpec
import org.gradle.nativeplatform.NativeLibrarySpec
import org.gradle.nativeplatform.internal.AbstractNativeBinarySpec

import org.doomedsociety.gradlecpp.GradleCppUtils

class CppUnitTestPlugin implements Plugin<Project> {

    private static class TestExecStatus {
        boolean successful
        int exitCode
        String output
        long durationMsec
        String cmdLine
        String execDir
    }

    static void onBinariesCreated(Project p, String desc, Closure action) {
        ModelRegistry mr = (p as AbstractProject).getModelRegistry()
        def modelPath = ModelPath.path("binaries")
        ModelRuleDescriptor ruleDescriptor = new SimpleModelRuleDescriptor(desc);

        mr.configure(ModelActionRole.Finalize, DirectNodeModelAction.of(ModelReference.of(modelPath), ruleDescriptor, new Action<MutableModelNode>() {
            @Override
            void execute(MutableModelNode node) {
                action()
            }
        }))
    }

    @Override
    void apply(Project project) {
        project.extensions.create('cppUnitTest', CppUnitTestExtension, project)
        onBinariesCreated(project, 'CppUnitTestPlugin::AttachUnitTest', {
            processCppUnitTests(project)
        })
    }



    /**
     * Attaches test tasks to C/C++ libraries build tasks
     */
    static void processCppUnitTests(Project p) {
        //println "processCppUnitTests::afterEvaluate on ${p.name}: project type is ${p.projectType}"

        p.binaries.all { NativeBinarySpec bin ->
            if (!(bin.component instanceof NativeLibrarySpec)) {
                return
            }

            def testComponentName = bin.component.name + '_tests'
            Collection<NativeBinarySpec> testCandidates = p.binaries.matching { it.component.name == testComponentName && bin.buildType == it.buildType && bin.flavor == it.flavor }
            if (testCandidates.size() > 1) {
                throw new GradleException("Found >1 test candidates for library ${bin.component.name} in project ${p}: ${testCandidates}")
            } else if (!testCandidates.empty) {
                def testBinary = testCandidates.first()
                GradleCppUtils.onTasksCreated(p, 'CppUnitTestPlugin::AttachUnitTestTask', {
                    attachTestTaskToCppLibrary(bin, testBinary)
                })
                String testTaskName = bin.namingScheme.getTaskName('unitTest')
                bin.ext.cppUnitTestTask = testTaskName
            } else {
                throw new GradleException("No tests found for library ${bin.component.name} in project ${p}")
            }
        }

    }

    static TestExecStatus runTestExecutable(NativeBinarySpec testSubject, String executable, List<String> params, String phase, int timeout) {
        def execFile = new File(executable)
        def outDir = new File(testSubject.buildTask.project.buildDir, "tests/${testSubject.name}/run")
        outDir.mkdirs()

        def outPath = new File(outDir, "${phase}.log")

        def cmdParams = [];
        cmdParams << execFile.absolutePath
        cmdParams.addAll(params)

        def execDir = execFile.parentFile
        def pb = new ProcessBuilder(cmdParams).redirectErrorStream(true).directory(execDir)
        if (!GradleCppUtils.windows) {
            pb.environment().put('LD_LIBRARY_PATH', '.')
        }


        def sout = new StringBuffer()

        long startTime = System.currentTimeMillis()
        def p = pb.start()
        p.consumeProcessOutput(sout, sout)

        p.waitForOrKill(timeout * 1000)
        long endTime = System.currentTimeMillis()

        int exitVal = p.exitValue()

        outPath.withWriter('UTF-8') { writer ->
            writer.write(sout.toString())
        }

        return new TestExecStatus(
                exitCode: exitVal,
                successful: (exitVal == 0),
                output: sout.toString(),
                durationMsec: endTime - startTime,
                cmdLine: cmdParams.join(' '),
                execDir: execDir.absolutePath
        )
    }

    static void dumpTestExecStatus(TestExecStatus stat) {
        if (!stat) {
            println "Execution of test executable failed"
        }

        println "Test executable command: ${stat.cmdLine}"
        println "Test executable run directury: ${stat.execDir}"
        println "Test executable exit code: ${stat.exitCode}"
        println "Test executable output BEGIN"
        println stat.output
        println "Test executable output END"
    }

    static void attachTestTaskToCppLibrary(NativeBinarySpec libBin, NativeBinarySpec testExecBin) {
        Project p = libBin.buildTask.project

        def libBinImpl = libBin as AbstractNativeBinarySpec
        def libLinkTask = GradleCppUtils.getLinkTask(libBin)
        def testExecLinkTask = GradleCppUtils.getLinkTask(testExecBin)

        // collect all output files from library and test executable
        def depFiles = []
        depFiles.addAll(libLinkTask.outputs.files.files)
        depFiles.addAll(testExecLinkTask.outputs.files.files)

        //create 'tests' task
        def testTaskName = libBinImpl.namingScheme.getTaskName('unitTest')
        def testTask = p.task(testTaskName, { Task testTask ->

            //output dir
            def testResDir = new File(p.buildDir, "tests/${libBin.name}")

            //inputs/outputs for up-to-date check
            testTask.outputs.dir testResDir
            testTask.inputs.files depFiles

            //dependencies on library and test executable
            testTask.dependsOn libLinkTask
            testTask.dependsOn testExecLinkTask

            // binary build depends on unit test
            libBin.buildTask.dependsOn testTask

            // extra project-specific dependencies
            def testDepsTask = p.tasks.findByName('testDeps')
            if (testDepsTask != null) {
                testTask.dependsOn testDepsTask
            }

            // task actions
            testTask.doLast {

                //temporary file that store info about all tests (XML)
                File allTests = File.createTempFile('j4s-testinfo', 'data')
                allTests.deleteOnExit()

                //fill file with test info
                print "Fetching test info..."
                def getTestsStatus = runTestExecutable(libBin, testExecBin.executableFile.absolutePath, ['-writeTestInfo', allTests.absolutePath], '__getTests', 5000)
                if (!getTestsStatus.successful) {
                    println " Failed"
                    dumpTestExecStatus(getTestsStatus)
                    throw new GradleException("Unable to fetch test names")
                }
                println " OK"
                getTestsStatus = null // allow GC to collect it

                // parse the test info file
                def root = new XmlSlurper().parse(allTests)

                // run all tests
                println "Running ${root.test.size()} tests..."
                TeamCityIntegration.suiteStarted("unitTests.${libBin.name}")
                int failCount = 0;
                root.test.list().each { testInfo ->
                    def testName = '' + testInfo.@name.text()
                    def testGroup = '' + testInfo.@group.text()
                    def testTimeout = ('' + testInfo.@timeout.text()) as int

                    if (!TeamCityIntegration.writeOutput) {
                        print "  ${testGroup}-${testName}..."
                        System.out.flush()
                    }

                    TeamCityIntegration.testStarted("${testGroup}-${testName}")
                    def testExecStatus = runTestExecutable(libBin, testExecBin.executableFile.absolutePath, ['-runTest', testGroup, testName],  "${testGroup}-${testName}", testTimeout)
                    if (!testExecStatus.successful) {
                        if (!TeamCityIntegration.writeOutput) {
                            println " Failed"
                        }

                        TeamCityIntegration.testFailed("${testGroup}-${testName}", "test executable return code is ${testExecStatus.exitCode}", "test executable return code is ${testExecStatus.exitCode}")
                        dumpTestExecStatus(testExecStatus)
                        failCount++
                    } else {
                        if (!TeamCityIntegration.writeOutput) {
                            println " OK"
                        }
                    }

                    TeamCityIntegration.testStdOut("${testGroup}-${testName}", testExecStatus.output)
                    TeamCityIntegration.testFinished("${testGroup}-${testName}", testExecStatus.durationMsec)

                }
                TeamCityIntegration.suiteFinished("unitTests.${libBin.name}")

                if (failCount) {
                    throw new GradleException("CPP unit tests: ${failCount} tests failed");
                }
            }
        })
    }
}
