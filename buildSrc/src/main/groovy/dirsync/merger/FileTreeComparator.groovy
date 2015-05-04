package dirsync.merger

import dirsync.model.synccmd.AbstractSyncCmd
import dirsync.model.synccmd.CopyDirCmd
import dirsync.model.synccmd.CopyFileCmd
import dirsync.model.synccmd.DeleteDirCmd
import dirsync.model.synccmd.DeleteFileCmd
import dirsync.model.synccmd.ReplaceFileCmd
import dirsync.model.tree.DirectoryNode
import dirsync.model.tree.FileNode
import groovy.transform.TypeChecked

@TypeChecked
class FileTreeComparator {

    private static <T, U> void mergeDirsRecursive(DirectoryNode<T> left, DirectoryNode<U> right, List<AbstractSyncCmd<T, U>> diffs) {

        // left => right
        left.childNodes.each { le ->
            def leftNode = le.value
            def rightNode = right.childNodes[leftNode.name]

            if (rightNode == null) {
                switch (leftNode) {
                    case DirectoryNode:
                        def leftDirNode = leftNode as DirectoryNode<T>
                        diffs << new CopyDirCmd<>(src: leftDirNode, dstParentDir: right)
                        break

                    case FileNode:
                        def leftFileNode = leftNode as FileNode<T>
                        diffs << new CopyFileCmd<>(src: leftFileNode, dstDir: right)
                        break

                    default:
                        throw new RuntimeException("Invalid node class ${leftNode.class.name}")
                }

                return
            }

            if (rightNode.class != leftNode.class) {
                throw new RuntimeException("node classes mismatch: ${leftNode.class.name} != ${rightNode.class.name}")
            }

            switch (rightNode) {
                case DirectoryNode:
                    def leftDirNode = leftNode as DirectoryNode<T>
                    def rightDirNode = rightNode as DirectoryNode<U>
                    mergeDirsRecursive(leftDirNode, rightDirNode, diffs)
                    break

                case FileNode:
                    def leftFileNode = leftNode as FileNode<T>
                    def rightFileNode = rightNode as FileNode<T>
                    if (leftFileNode.size != rightFileNode.size || leftFileNode.lastModifiedDate != rightFileNode.lastModifiedDate) {
                        diffs << new ReplaceFileCmd<>(src: leftFileNode, dst: rightFileNode)
                    }
                    break

                default:
                    throw new RuntimeException("Invalid node class ${rightNode.class.name}")
            }
        } // ~left => right

        //right => left
        right.childNodes.each { re ->
            def rightNode = re.value
            def leftNode = left.childNodes[rightNode.name]

            if (leftNode != null) {
                return //already processed in left => right
            }

            switch (rightNode) {
                case DirectoryNode:
                    def rightDirNode = rightNode as DirectoryNode<U>
                    diffs << new DeleteDirCmd<>(dirNode: rightDirNode)
                    break

                case FileNode:
                    def rightFileNode = rightNode as FileNode<T>
                    diffs << new DeleteFileCmd<>(node: rightFileNode)
                    break

                default:
                    throw new RuntimeException("Invalid node class ${rightNode.class.name}")
            }
        } // ~right => left
    }

    static <T, U> List<AbstractSyncCmd<T, U>> mergeTrees(DirectoryNode<T> leftRoot, DirectoryNode<U> rightRoot) {
        List<AbstractSyncCmd<T, U>> res = []
        mergeDirsRecursive(leftRoot, rightRoot, res)
        return res
    }
}
