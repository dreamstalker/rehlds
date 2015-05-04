package dirsync.builder

import org.junit.Test

import java.io.File

import dirsync.builder.ZipTreeBuilder

import java.util.zip.ZipEntry
import java.util.zip.ZipFile
import java.util.zip.ZipOutputStream;

import static org.junit.Assert.*;

class ZipTreeBuilderTest {

    @Test
    void test1() {
        File zipFile = File.createTempFile('ZipTreeBuilderTest', 'zip')
        zipFile.deleteOnExit()

        new ZipOutputStream(zipFile.newDataOutputStream()).withStream { ZipOutputStream zos ->
            zos.putNextEntry(new ZipEntry('aRootFile1.txt'))
            zos.write(65) //'A'

            zos.putNextEntry(new ZipEntry('dir1/'))
            zos.putNextEntry(new ZipEntry('dir1/dir2/'))

            zos.putNextEntry(new ZipEntry('dir1/dir2/d1d2f1.txt'))
            zos.write(65); zos.write(66) //'AB'

            zos.putNextEntry(new ZipEntry('dir1/d1f1.txt'))
            zos.write(65); zos.write(66); zos.write(67) //'ABC'

            zos.putNextEntry(new ZipEntry('zRootFile2.txt'))
            zos.write(65); zos.write(66); zos.write(67); zos.write(68) //'ABCD'
        }

        ZipFile zf = new ZipFile(zipFile.absolutePath)
        def tree = ZipTreeBuilder.buildForZipArchive(zipFile.absolutePath, zf)

        assert tree.childNodes.size() == 3
    }
}