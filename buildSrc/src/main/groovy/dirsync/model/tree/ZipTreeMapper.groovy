package dirsync.model.tree

import dirsync.builder.FileTreeMerger
import dirsync.builder.ZipTreeBuilder
import sun.reflect.generics.reflectiveObjects.NotImplementedException

import java.util.zip.ZipFile

public class ZipTreeMapper extends TreePhysMapper<ZipData> implements Closeable {
    Map<String, ZipFile> zipArchives = [:]

    void addZipArchive(String zipArchive) {
        zipArchives[zipArchive] = new ZipFile(zipArchive)
    }

    DirectoryNode<ZipData> buildFileTree() {
        def root = new DirectoryNode<ZipData>()
        zipArchives.each { ze ->
            def zipTree = ZipTreeBuilder.buildForZipArchive(ze.key, ze.value)
            root = FileTreeMerger.mergeTrees(root, zipTree)
        }

        return root
    }

    @Override
    void close() throws IOException {
        zipArchives.each { ze ->
            try { ze.value.close() } catch (Exception ignored) { }
        }
    }

    @Override
    InputStream fileContent(ZipData file) {
        def archive = zipArchives[file.zipArchiveName]
        if (!archive) {
            throw new RuntimeException("Archive ${file.zipArchiveName} is not loaded");
        }

        def zipEntry = archive.getEntry(file.zipEntryName)
        if (!zipEntry) {
            throw new RuntimeException("File ${file.zipEntryName} not found in archive ${file.zipArchiveName}");
        }

        return archive.getInputStream(zipEntry)
    }

    @Override
    void createDirectory(String dir) {
        throw new NotImplementedException()
    }

    @Override
    void removeDirectory(String dir) {
        throw new NotImplementedException()
    }

    @Override
    void removeFile(String path) {
        throw new NotImplementedException()
    }

    @Override
    OutputStream createFile(String path) {
        throw new NotImplementedException()
    }

    @Override
    void setFileLastUpdatedDate(String path, long date) {
        throw new NotImplementedException()
    }
}
