package gradlecpp

import gradlecpp.teamcity.TeamCityIntegration
import org.apache.commons.lang.SystemUtils
import org.gradle.api.DefaultTask
import org.gradle.api.file.FileCollection
import org.gradle.api.tasks.TaskAction
import org.gradle.nativeplatform.NativeBinarySpec
import rehlds.testdemo.RehldsDemoRunner
import rehlds.testdemo.RehldsTestParser

class RehldsPlayTestTask extends DefaultTask {

    def FileCollection testDemos
    def Closure postExtractAction
    def File rehldsImageRoot
    def File rehldsTestLogs
    def NativeBinarySpec testFor

    @TaskAction
    def doPlay() {
        if (!SystemUtils.IS_OS_WINDOWS) {
            return
        }

        if (!testDemos) {
            println 'RehldsPlayTestTask: no demos attached to the testDemos property'
        }

        rehldsImageRoot.mkdirs()
        rehldsTestLogs.mkdirs()

        def demoRunner = new RehldsDemoRunner(this.project.configurations.rehlds_playtest_image.getFiles(), rehldsImageRoot, postExtractAction)

        println "Preparing engine..."
        demoRunner.prepareEngine()

        println "Running ${testDemos.getFiles().size()} ReHLDS test demos..."

        TeamCityIntegration.suiteStarted("rehldsDemo.${testFor.name}")
        int failCount = 0;
        testDemos.getFiles().each { f ->
            def testInfo = RehldsTestParser.parseTestInfo(f)
            TeamCityIntegration.testStarted(testInfo.testName)

            if (!TeamCityIntegration.writeOutput) {
                print "Running ReHLDS test demo ${testInfo.testName} "
                System.out.flush()
            }


            def testRes = demoRunner.runTest(testInfo, rehldsTestLogs)

            if (testRes.success) {
                if (!TeamCityIntegration.writeOutput) {
                    println ' OK'
                }
            } else {

                TeamCityIntegration.testFailed(testInfo.testName, "Exit code: ${testRes.returnCode}", "Exit code: ${testRes.returnCode}")
                if (!TeamCityIntegration.writeOutput) {
                    println ' Failed'
                    println "ReHLDS testdemo ${testInfo.testName} playback failed. Exit status is ${testRes.returnCode}."
                    println "Dumping console output:"
                    println testRes.hldsConsoleOutput
                }

                failCount++
            }

            TeamCityIntegration.testStdOut(testInfo.testName, testRes.hldsConsoleOutput)
            TeamCityIntegration.testFinished(testInfo.testName, testRes.duration)
        }
        TeamCityIntegration.suiteFinished("rehldsDemo.${testFor.name}")

        if (failCount) {
            throw new RuntimeException("Rehlds testdemos: failed ${failCount} tests")
        }
    }
}
