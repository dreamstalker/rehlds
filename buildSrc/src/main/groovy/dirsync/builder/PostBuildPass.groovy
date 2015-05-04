package dirsync.builder

import dirsync.model.tree.DirectoryNode

class PostBuildPass {

    private static <T> void postProcessRecursive(DirectoryNode<T> dir) {
        dir.childNodes.each { cne ->
            def childNode = cne.value
            childNode.fullPath = dir.fullPath ? dir.fullPath + '/' + childNode.name : childNode.name
            if (childNode instanceof DirectoryNode) {
                def childDirNode = childNode as DirectoryNode<T>
                postProcessRecursive(childDirNode)
            }
        }
    }

    static <T> void doPostBuild(DirectoryNode<T> root) {
        root.fullPath = ''
        postProcessRecursive(root)
    }
}
