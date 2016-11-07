package versioning

import java.util.Set;

import groovy.transform.CompileStatic
import groovy.transform.TypeChecked
import org.eclipse.jgit.api.Git
import org.eclipse.jgit.api.Status;
import org.eclipse.jgit.lib.ObjectId
import org.eclipse.jgit.lib.Repository
import org.eclipse.jgit.lib.StoredConfig
import org.eclipse.jgit.revwalk.RevCommit
import org.eclipse.jgit.revwalk.RevWalk
import org.eclipse.jgit.storage.file.FileRepositoryBuilder
import org.joda.time.DateTime
import org.joda.time.DateTimeZone

@CompileStatic @TypeChecked
class GitVersioner {

    static GitInfo versionForDir(String dir) {
        versionForDir(new File(dir))
    }
    static int getCountCommit(Repository repo) {
        Iterable<RevCommit> commits = Git.wrap(repo).log().call()
        int count = 0;
        commits.each {
            count++;
        }

        return count;
    }
    static String prepareUrlToCommits(String url) {
        if (url == null) {
            // default remote url
            return "https://github.com/dreamstalker/rehlds/commit/";
        }

        StringBuilder sb = new StringBuilder();
        String childPath;
        int pos = url.indexOf('@');
        if (pos != -1) {
            childPath = url.substring(pos + 1, url.lastIndexOf('.git')).replace(':', '/');
            sb.append('https://');
        } else {
            pos = url.lastIndexOf('.git');
            childPath = (pos == -1) ? url : url.substring(0, pos);
        }

        // support for different links to history of commits
        if (url.indexOf('bitbucket.org') != -1) {
            sb.append(childPath).append('/commits/');
        } else {
            sb.append(childPath).append('/commit/');
        }
        return sb.toString();
    }
    // check uncommited changes
    static boolean getUncommittedChanges(Repository repo) {
        Git git = new Git(repo);
        Status status = git.status().call();

        Set<String> uncommittedChanges = status.getUncommittedChanges();
        for(String uncommitted : uncommittedChanges) {
            return true;
        }

        return false;
    }
    static GitInfo versionForDir(File dir) {
        FileRepositoryBuilder builder = new FileRepositoryBuilder()
        Repository repo = builder.setWorkTree(dir)
                .findGitDir()
                .build()

        ObjectId head = repo.resolve('HEAD')
        if (!head) {
            return null
        }

        final StoredConfig cfg = repo.getConfig();

        def commit = new RevWalk(repo).parseCommit(head)
        if (!commit) {
            throw new RuntimeException("Can't find last commit.")
        }

        def localChanges = getUncommittedChanges(repo);
        def commitDate = new DateTime(1000L * commit.commitTime, DateTimeZone.UTC);
        if (localChanges) {
            commitDate = new DateTime();
        }

        def branch = repo.getBranch()

        String url = null;
        String remote_name = cfg.getString("branch", branch, "remote");

        if (remote_name == null) {
            for (String remotes : cfg.getSubsections("remote")) {
                if (url != null) {
                    println 'Found a second remote: (' + remotes + '), url: (' + cfg.getString("remote", remotes, "url") + ')'
                    continue;
                }

                url = cfg.getString("remote", remotes, "url");
            }
        } else {
            url = cfg.getString("remote", remote_name, "url");
        }

        String commitURL = prepareUrlToCommits(url);
        String tag = repo.tags.find { kv -> kv.value.objectId == commit.id }?.key
        String commitSHA = commit.getId().abbreviate(7).name();

        return new GitInfo(
            localChanges: localChanges,
            commitDate: commitDate,
            branch: branch,
            tag: tag,
            commitSHA: commitSHA,
            commitURL: commitURL,
            commitCount: getCountCommit(repo)
        )
    }
}
