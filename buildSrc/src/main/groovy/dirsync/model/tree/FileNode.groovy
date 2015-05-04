package dirsync.model.tree

import groovy.transform.CompileStatic

@CompileStatic
class FileNode<T> extends AbstractFileTreeNode<T> {
    long size
}
