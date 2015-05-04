package gradlecpp

import org.gradle.api.Plugin
import org.gradle.api.Project

class RehldsPlayTestPlugin implements Plugin<Project> {
    @Override
    void apply(Project project) {
        project.configurations {
            rehlds_playtest_image
        }

        project.dependencies {
            rehlds_playtest_image 'rehlds.testimg:testimg:0.2'
        }
    }
}
