package dirsync.model.synccmd

import dirsync.model.tree.DirectoryNode

class DeleteDirCmd<T, U> extends AbstractSyncCmd<T, U> {
    DirectoryNode<U> dirNode
}
