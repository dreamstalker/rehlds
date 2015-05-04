package dirsync.model.synccmd

import dirsync.model.tree.DirectoryNode
import dirsync.model.tree.FileNode

class CopyFileCmd<T, U> extends AbstractSyncCmd<T, U> {
    FileNode<T> src
    DirectoryNode<U> dstDir
}
