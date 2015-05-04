package gradlecpp

import org.gradle.api.Project
import org.gradle.nativeplatform.NativeBinarySpec

class CppUnitTestExtension {
    Project _project

    CppUnitTestExtension(Project p) {
        _project = p
    }

    void eachTestExecutable(Closure action) {
        _project.binaries.each { NativeBinarySpec bin ->
            if (!bin.hasProperty('cppUnitTestsExecutable')) return
            action(bin)
        }
    }
}
