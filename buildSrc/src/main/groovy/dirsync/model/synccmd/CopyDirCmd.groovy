package dirsync.model.synccmd

import dirsync.model.tree.DirectoryNode

class CopyDirCmd<T, U> extends AbstractSyncCmd<T, U> {
    DirectoryNode<T> src
    DirectoryNode<U> dstParentDir
}
