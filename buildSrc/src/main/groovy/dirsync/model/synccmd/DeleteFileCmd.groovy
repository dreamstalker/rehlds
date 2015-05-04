package dirsync.model.synccmd

import dirsync.model.tree.FileNode

class DeleteFileCmd<T, U> extends AbstractSyncCmd<T, U> {
    FileNode<U> node
}
