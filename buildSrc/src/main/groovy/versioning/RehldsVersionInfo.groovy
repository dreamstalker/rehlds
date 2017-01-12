package versioning

import groovy.transform.CompileStatic
import groovy.transform.ToString
import groovy.transform.TypeChecked
import org.joda.time.format.DateTimeFormat
import org.joda.time.DateTime

@CompileStatic @TypeChecked
@ToString(includeNames = true)
class RehldsVersionInfo {
    int majorVersion
    int minorVersion
    Integer maintenanceVersion
    String suffix

    boolean  localChanges
    DateTime commitDate
    String   commitSHA
    String   commitURL
    Integer  commitCount

    String asMavenVersion(boolean extra = true) {
        StringBuilder sb = new StringBuilder()
        sb.append(majorVersion).append('.' + minorVersion);
        if (maintenanceVersion != null) {
            sb.append('.' + maintenanceVersion);
        }

        if (commitCount != null) {
            sb.append('.' + commitCount)
        }

        if (extra && suffix) {
            sb.append('-' + suffix)
        }

        // do mark for this build like a modified version
        if (extra && localChanges) {
            sb.append('+m');
        }

        return sb.toString()
    }
    String asCommitDate() {
        String pattern = "MMM  d yyyy";
        if (commitDate.getDayOfMonth() >= 10) {
            pattern = "MMM d yyyy";
        }

        return DateTimeFormat.forPattern(pattern).withLocale(Locale.ENGLISH).print(commitDate);
    }
    String asCommitTime() {
        return DateTimeFormat.forPattern('HH:mm:ss').withLocale(Locale.ENGLISH).print(commitDate);
    }
}
