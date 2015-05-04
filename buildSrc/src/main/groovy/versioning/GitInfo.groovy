package versioning

import groovy.transform.CompileStatic
import groovy.transform.TypeChecked
import org.joda.time.DateTime

@CompileStatic @TypeChecked
class GitInfo {
    DateTime lastCommitDate
    String branch
    String tag
}
