package dirsync.model.tree

class FSMapper extends TreePhysMapper<File> {
    final File root

    FSMapper(File root) {
        this.root = root
    }

    @Override
    InputStream fileContent(File file) {
        return file.newDataInputStream()
    }

    @Override
    void createDirectory(String dir) {
        def target = new File(root, dir)
        if (!target.mkdirs()) {
            throw new RuntimeException("Failed to create directory ${target.absolutePath}")
        }
    }

    @Override
    void removeDirectory(String dir) {
        def target = new File(root, dir)
        if (!target.deleteDir()) {
            throw new RuntimeException("Failed to delete directory ${target.absolutePath}")
        }
    }

    @Override
    void removeFile(String path) {
        def target = new File(root, path)
        if (!target.delete()) {
            throw new RuntimeException("Failed to delete file ${target.absolutePath}")
        }
    }

    @Override
    OutputStream createFile(String path) {
        def target = new File(root, path)
        return target.newOutputStream()
    }

    @Override
    void setFileLastUpdatedDate(String path, long date) {
        def target = new File(root, path)
        target.setLastModified(date)
    }
}
