package dirsync.builder

import dirsync.model.tree.DirectoryNode
import dirsync.model.tree.FileNode

class FileTreeMerger {

    private static <T> void mergeContentsRecursive(DirectoryNode<T> newParent, DirectoryNode<T> toMerge) {
        toMerge.childNodes.each { cn ->
            def node = cn.value
            def existingNode = newParent.childNodes[node.name]
            if (existingNode) {
                if (!(existingNode instanceof DirectoryNode) || !(node instanceof DirectoryNode))
                    throw new RuntimeException("Failed to merge non-directory nodes ${node.fullPath}")

                def existingDirNode = existingNode as DirectoryNode<T>
                def dirNode = node as DirectoryNode<T>

                existingDirNode.lastModifiedDate = Math.max(existingDirNode.lastModifiedDate, dirNode.lastModifiedDate)
                mergeContentsRecursive(existingDirNode, dirNode)
            } else {
                if (node instanceof DirectoryNode) {
                    def dirNode = node as DirectoryNode<T>
                    def newNode = new DirectoryNode<T>(
                            name: dirNode.name,
                            data: dirNode.data,
                            parent: newParent,
                            lastModifiedDate: dirNode.lastModifiedDate
                    )
                    newParent.childNodes[node.name] = newNode

                    mergeContentsRecursive(newNode, dirNode)
                } else {
                    FileNode<T> fileNode = node as FileNode<T>
                    FileNode<T> newNode = new FileNode<T>(
                            name: fileNode.name,
                            data: fileNode.data,
                            parent: newParent,
                            lastModifiedDate: fileNode.lastModifiedDate,
                            size: fileNode.size
                    )

                    newParent.childNodes[node.name] = newNode
                }
            }
        }
    }

    public static <T> DirectoryNode<T> mergeTrees(DirectoryNode<T> tree1, DirectoryNode<T> tree2) {
        DirectoryNode<T> newRoot = new DirectoryNode<T>(
                name: tree1.name ?: tree2.name
        )

        mergeContentsRecursive(newRoot, tree1)
        mergeContentsRecursive(newRoot, tree2)
        PostBuildPass.doPostBuild(newRoot)

        return newRoot
    }
}
