package dirsync.builder

import dirsync.model.tree.DirectoryNode
import dirsync.model.tree.FileNode
import dirsync.model.tree.ZipData

import java.util.zip.ZipFile

class ZipTreeBuilder {
    static DirectoryNode<ZipData> buildForZipArchive(String zipArchive, ZipFile zf) {
        DirectoryNode<ZipData> root = new DirectoryNode<>()

        zf.entries().each { ze ->
            def path = ze.name.replace('\\', '/')
            if (path.endsWith('/'))
                path = path.substring(0, path.length() - 1)

            def parentPath = path.contains('/') ? path.substring(0, path.lastIndexOf('/')) : ''
            def childPath = path.contains('/') ? path.substring(path.lastIndexOf('/') + 1) : path

            def parentNode = (DirectoryNode<ZipData>) root.getByPath(parentPath)
            if (parentNode == null)
                throw new RuntimeException("Error reading ${zipArchive}: could not find parent path ${parentPath} for path ${path}")

            def childNode = parentNode.getChildren(childPath)
            if (childNode)
                throw new RuntimeException("Error reading ${zipArchive}: duplicate path ${path}")

            if (ze.directory) {
                childNode = new DirectoryNode<ZipData>(
                        name: childPath,
                        lastModifiedDate: ze.time,
                        data: new ZipData(zipEntryName: ze.name, zipArchiveName: zipArchive),
                        parent: parentNode
                );
            } else {
                childNode = new FileNode<ZipData>(
                        name: childPath,
                        lastModifiedDate: ze.time,
                        data: new ZipData(zipEntryName: ze.name, zipArchiveName: zipArchive),
                        parent: parentNode,
                        size: ze.size
                );
            }
            parentNode.childNodes[childPath] = childNode

            //println '' + ze.directory + '   ' + ze.name + '   ' + parentPath + '   ' + childPath
        }

        PostBuildPass.doPostBuild(root)
        return root
    }
}
