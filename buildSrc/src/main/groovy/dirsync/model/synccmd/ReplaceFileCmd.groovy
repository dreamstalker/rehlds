package dirsync.model.synccmd

import dirsync.model.tree.FileNode

class ReplaceFileCmd<T, U> extends AbstractSyncCmd<T, U> {
    FileNode<T> src
    FileNode<U> dst
}
