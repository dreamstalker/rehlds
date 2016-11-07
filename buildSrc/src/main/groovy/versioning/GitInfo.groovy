package versioning

import groovy.transform.CompileStatic
import groovy.transform.TypeChecked
import org.joda.time.DateTime

@CompileStatic @TypeChecked
class GitInfo {
    boolean localChanges
    DateTime commitDate
    String branch
    String tag
    String commitSHA
    String commitURL
    Integer commitCount
}
