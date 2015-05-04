package dirsync.model.tree

import groovy.transform.CompileStatic

@CompileStatic
abstract class AbstractFileTreeNode<T> {
    DirectoryNode<T> parent
    String name
    String fullPath
    long lastModifiedDate
    T data

    boolean equals(o) {
        if (this.is(o)) return true
        if (getClass() != o.class) return false

        AbstractFileTreeNode that = (AbstractFileTreeNode) o

        if (name != that.name) return false

        return true
    }

    int hashCode() {
        return (name != null ? name.hashCode() : 0)
    }
}
