package org.rehlds.flightrec.filescan;

import com.google.cloud.Crc32c;
import org.rehlds.flightrec.api.util.UtilByteBuffer;
import org.rehlds.flightrec.api.util.UtilSizeBuf;
import static org.rehlds.flightrec.Consts.*;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Arrays;
import java.util.List;



public class FlightRecFileScanner {
    RandomAccessFile file;
    long fileLen;
    FileScanResult scanRes = new FileScanResult();

    private byte readBuf[] = new byte[65536];
    private byte header[] = new byte[MAX_HEADER_SIZE];
    private UtilSizeBuf headerSBuf = new UtilSizeBuf("header", new UtilByteBuffer(header), 0, header.length);

    private FlightRecFileScanner(RandomAccessFile file) throws IOException {
        this.file = file;
        this.fileLen = file.length();
    }

    private void examineHeader(byte[] data, int size, int pos) throws IOException {
        if (pos + MAX_HEADER_SIZE < size) {
            System.arraycopy(data, pos, header, 0, MAX_HEADER_SIZE);
        } else {
            return; //will be read in next iteration
        }

        headerSBuf.reset();
        String matchedType = null;
        if (Arrays.equals(META_HEADER_SIG_BYTES, Arrays.copyOfRange(header, 0, META_HEADER_SIG_BYTES.length))) {
            matchedType = META_HEADER_SIG_STR;
            headerSBuf.skip(META_HEADER_SIG_BYTES.length);
        } else if (Arrays.equals(DATA_HEADER_SIG_BYTES, Arrays.copyOfRange(header, 0, DATA_HEADER_SIG_BYTES.length))) {
            matchedType = DATA_HEADER_SIG_STR;
            headerSBuf.skip(DATA_HEADER_SIG_BYTES.length);
        }

        if (matchedType == null) {
            return;
        }

        List<HeaderScanResult> resList = (matchedType.equals(META_HEADER_SIG_STR)) ? scanRes.metaHeaders : scanRes.dataHeaders;
        int version = headerSBuf.readInt32();
        int allocSize = headerSBuf.readInt32();

        Crc32c crc32 = new Crc32c();
        crc32.update(header, 0, headerSBuf.tell());
        long calculatedChecksum = crc32.getValue();
        long bufChecksum = headerSBuf.readUInt32();

        if (calculatedChecksum != bufChecksum) {
            resList.add(new HeaderScanResult(file.getFilePointer() - size + pos, allocSize, false, "Checksum mismatch", version));
            return;
        }

        long endPos = file.getFilePointer() - size + pos + allocSize;
        if (endPos > file.length()) {
            resList.add(new HeaderScanResult(file.getFilePointer() - size + pos, allocSize, false, "Regions partially lays outside the file", version));
            return;
        }
        resList.add(new HeaderScanResult(file.getFilePointer() - size + pos, allocSize, true, null, version));
    }

    private void scanForHeaders(byte[] data, int size) throws IOException {
        int maxHeaderSize = Math.max(META_HEADER_SIG_STR.length(), DATA_HEADER_SIG_STR.length());
        for (int i = 0; i < size - maxHeaderSize; i++) {
            if (data[i + 15] == META_HEADER_SIG_BYTES[15] && data[i + 16] == META_HEADER_SIG_BYTES[16] && data[i + 17] == META_HEADER_SIG_BYTES[17] && data[i + 18] == META_HEADER_SIG_BYTES[18]) {
                examineHeader(data, size, i);
            } else if (data[i + 15] == DATA_HEADER_SIG_BYTES[15] && data[i + 16] == DATA_HEADER_SIG_BYTES[16] && data[i + 17] == DATA_HEADER_SIG_BYTES[17] && data[i + 18] == DATA_HEADER_SIG_BYTES[18]) {
                examineHeader(data, size, i);
            }
        }
    }

    private void doScan() throws IOException {
        file.seek(0);
        int read;

        while (-1 != (read = file.read(readBuf))) {
            scanForHeaders(readBuf, read);
            if (read == readBuf.length) {
                file.seek(file.getFilePointer() - MAX_HEADER_SIZE * 2);
            }
        }
    }

    public static FileScanResult scan(RandomAccessFile file) throws IOException {
        FlightRecFileScanner scanner = new FlightRecFileScanner(file);
        scanner.doScan();
        return scanner.scanRes;
    }
}
