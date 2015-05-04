package dirsync.model.tree

import groovy.transform.CompileStatic

@CompileStatic
class DirectoryNode<T> extends AbstractFileTreeNode<T> {
    Map<String, AbstractFileTreeNode<T>> childNodes = new HashMap<>()

    AbstractFileTreeNode<T> getChildren(String name) {
        return childNodes[name];
    }

    AbstractFileTreeNode<T> getChildren(String[] names, int idx) {
        if (idx == names.length)
            return this

        AbstractFileTreeNode<T> c = childNodes[names[idx]]
        if (c == null)
            return null

        if (c instanceof DirectoryNode) {
            def d = (DirectoryNode<T>) c;
            return d.getChildren(names, idx + 1)
        }

        return null;
    }

    AbstractFileTreeNode<T> getByPath(String path) {
        path = path.replace('\\', '/')
        if (path.endsWith('/'))
            path = path.substring(0, path.length() - 1)

        if (path.empty) {
            return this
        }

        String[] components = path.split('/')
        return getChildren(components, 0)
    }

}
