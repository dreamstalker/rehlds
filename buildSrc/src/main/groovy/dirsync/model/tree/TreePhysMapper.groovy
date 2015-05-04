package dirsync.model.tree

abstract class TreePhysMapper<T> {
    abstract InputStream fileContent(T file)
    abstract void createDirectory(String dir)
    abstract void removeDirectory(String dir)
    abstract void removeFile(String path)
    abstract OutputStream createFile(String path)

    abstract void setFileLastUpdatedDate(String path, long date)
}
