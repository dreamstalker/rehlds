package dirsync.builder

import dirsync.model.tree.DirectoryNode
import dirsync.model.tree.FileNode
import groovy.transform.CompileStatic

class FileSystemTreeBuilder {

    @CompileStatic
    private static FileNode<File> buildNodeForFile(File file, DirectoryNode<File> parent) {
        if (parent.getChildren(file.name)) {
            throw new RuntimeException("Parent dir ${parent.name} already contains child node ${file.name}");
        }

        return new FileNode(
                name: file.name,
                lastModifiedDate: file.lastModified(),
                data: file,
                parent: parent,
                size: file.size()
        );
    }

    @CompileStatic
    private static DirectoryNode<File> buildNodeForDirectoryRecursive(File dir, DirectoryNode<File> parent) {
        if (!dir.isDirectory()) {
            throw new RuntimeException("File ${dir.absolutePath} is not a directory")
        }

        if (parent != null && parent.getChildren(dir.name)) {
            throw new RuntimeException("Parent dir ${parent.name} already contains child node ${dir.name}");
        }

        DirectoryNode<File> thisNode = new DirectoryNode(
                name: dir.name,
                lastModifiedDate: dir.lastModified(),
                data: dir,
                parent: parent
        );

        dir.eachFile { File f ->
            if (f.isDirectory()) {
                thisNode.childNodes[f.name] = buildNodeForDirectoryRecursive(f, thisNode)
            } else {
                thisNode.childNodes[f.name] = buildNodeForFile(f, thisNode)
            }
        }

        return thisNode;
    }

    static DirectoryNode<File> buildFileSystemTree(File rootDir) {
        def root = buildNodeForDirectoryRecursive(rootDir, null);
        PostBuildPass.doPostBuild(root)

        return root
    }
}
