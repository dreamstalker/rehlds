package dirsync.merger

import dirsync.model.synccmd.AbstractSyncCmd
import dirsync.model.synccmd.CopyDirCmd
import dirsync.model.synccmd.CopyFileCmd
import dirsync.model.synccmd.DeleteDirCmd
import dirsync.model.synccmd.DeleteFileCmd
import dirsync.model.synccmd.ReplaceFileCmd
import dirsync.model.tree.DirectoryNode
import dirsync.model.tree.FileNode
import dirsync.model.tree.TreePhysMapper
import groovy.transform.TypeChecked
import org.apache.commons.io.IOUtils

@TypeChecked
public class FileTreeDiffApplier {

    static <T, U> void copyDirRecursive(DirectoryNode<T> src, TreePhysMapper<T> srcMapper, TreePhysMapper<U> dstMapper) {
        dstMapper.createDirectory(src.fullPath)
        src.childNodes.each { ce ->
            def childNode = ce.value
            def childPath = childNode.fullPath
            switch (childNode) {
                case FileNode:
                    srcMapper.fileContent(childNode.data).withStream { InputStream inStream ->
                        dstMapper.createFile(childPath).withStream { OutputStream outStream ->
                            IOUtils.copy(inStream, outStream)
                        }

                        dstMapper.setFileLastUpdatedDate(childPath, childNode.lastModifiedDate)
                    }
                    break;

                case DirectoryNode:
                    copyDirRecursive(childNode as DirectoryNode<T>, srcMapper, dstMapper)
                    break;

                default:
                    throw new RuntimeException("Invalid node class: ${childNode.class.name}")
            }
        }
    }

    static <T, U> void handleCopyFile(CopyFileCmd<T, U> fileCopy, TreePhysMapper<T> srcMapper, TreePhysMapper<U> dstMapper) {
        def dstPath = fileCopy.dstDir.fullPath ? fileCopy.dstDir.fullPath + '/' + fileCopy.src.name : fileCopy.src.name
        srcMapper.fileContent(fileCopy.src.data).withStream { InputStream inStream ->
            dstMapper.createFile(dstPath).withStream { OutputStream outStream ->
                IOUtils.copy(inStream, outStream)
            }

            dstMapper.setFileLastUpdatedDate(dstPath, fileCopy.src.lastModifiedDate)
        }
    }

    static <T, U> void handleDeleteDir(DeleteDirCmd<T, U> delDir, TreePhysMapper<T> srcMapper, TreePhysMapper<U> dstMapper) {
        dstMapper.removeDirectory(delDir.dirNode.fullPath)
    }

    static <T, U> void handleDeleteFile(DeleteFileCmd<T, U> delFile, TreePhysMapper<T> srcMapper, TreePhysMapper<U> dstMapper) {
        dstMapper.removeFile(delFile.node.fullPath)
    }

    static <T, U> void handleReplaceFile(ReplaceFileCmd<T, U> replaceFile, TreePhysMapper<T> srcMapper, TreePhysMapper<U> dstMapper) {
        dstMapper.removeFile(replaceFile.dst.fullPath)
        srcMapper.fileContent(replaceFile.src.data).withStream { InputStream inStream ->
            dstMapper.createFile(replaceFile.dst.fullPath).withStream { OutputStream outStream ->
                IOUtils.copy(inStream, outStream)
            }

            dstMapper.setFileLastUpdatedDate(replaceFile.dst.fullPath, replaceFile.src.lastModifiedDate)
        }
    }

    static <T, U> void applyDiffs(List<AbstractSyncCmd<T, U>> diffs, TreePhysMapper<T> srcMapper, TreePhysMapper<U> dstMapper) {
        diffs.each { diff ->
            switch (diff) {
                case CopyDirCmd:
                    def copyDir = diff as CopyDirCmd<T, U>
                    copyDirRecursive(copyDir.src, srcMapper, dstMapper)
                    break

                case CopyFileCmd:
                    handleCopyFile(diff as CopyFileCmd<T, U>, srcMapper, dstMapper)
                    break

                case DeleteDirCmd:
                    handleDeleteDir(diff as DeleteDirCmd<T, U>, srcMapper, dstMapper)
                    break

                case DeleteFileCmd:
                    handleDeleteFile(diff as DeleteFileCmd<T, U>, srcMapper, dstMapper)
                    break

                case ReplaceFileCmd:
                    handleReplaceFile(diff as ReplaceFileCmd<T, U>, srcMapper, dstMapper)
                    break

                default:
                    throw new RuntimeException("Invalid diff command ${diff.class.name}")
            }
        }
    }
}