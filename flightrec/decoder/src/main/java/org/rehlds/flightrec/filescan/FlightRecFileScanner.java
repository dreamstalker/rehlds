package org.rehlds.flightrec.filescan;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Arrays;
import java.util.List;
import java.util.zip.CRC32;

public class FlightRecFileScanner {
    RandomAccessFile file;
    long fileLen;
    FileScanResult scanRes = new FileScanResult();

    private final static int HEADER_SIZE = 128;
    private final static String META_HEADER = "REHLDS_FLIGHTREC_META";
    private final static String DATA_HEADER = "REHLDS_FLIGHTREC_DATA";
    //                                         012345678901234567890
    //                                         000000000011111111112

    private static byte[] META_HEADER_SIGNATURE = (META_HEADER + META_HEADER + META_HEADER + ":").getBytes();
    private static byte[] DATA_HEADER_SIGNATURE = (META_HEADER + META_HEADER + META_HEADER + ":").getBytes();

    private byte readBuf[] = new byte[65536];
    private byte header[] = new byte[HEADER_SIZE];

    private FlightRecFileScanner(RandomAccessFile file) throws IOException {
        this.file = file;
        this.fileLen = file.length();
    }

    private void examineHeader(byte[] data, int size, int pos) throws IOException {
        if (pos + HEADER_SIZE < size) {
            System.arraycopy(data, pos, header, 0, HEADER_SIZE);
        } else {
            return; //will be read in next iteration
        }

        String matchedType = null;
        if (Arrays.equals(META_HEADER_SIGNATURE, Arrays.copyOfRange(header, 0, META_HEADER_SIGNATURE.length))) {
            matchedType = META_HEADER;
        } else if (Arrays.equals(DATA_HEADER_SIGNATURE, Arrays.copyOfRange(header, 0, DATA_HEADER_SIGNATURE.length))) {
            matchedType = DATA_HEADER;
        }

        if (matchedType == null) {
            return;
        }
        List<HeaderScanResult> resList = (matchedType.equals(META_HEADER)) ? scanRes.metaHeaders : scanRes.dataHeaders;
        int versionPos = matchedType.length() * 3 + 1;
        int version = header[versionPos] | header[versionPos+1] << 8 | header[versionPos+2] << 16 | header[versionPos+3] << 24;

        int allocSizePos = matchedType.length() * 3 + 1 + 4;
        int allocSize = header[allocSizePos] | header[allocSizePos+1] << 8 | header[allocSizePos+2] << 16 | header[allocSizePos+3] << 24;

        CRC32 crc32 = new CRC32();
        int bufCksumPos = matchedType.length() * 3 + 1 + 4 + 4; //3*head + ":" + version + allocsize
        crc32.update(header, 0, bufCksumPos);
        long calculatedChecksum = crc32.getValue();
        long bufChecksum = header[bufCksumPos] | header[bufCksumPos+1] << 8 | header[bufCksumPos+2] << 16 | header[bufCksumPos+3] << 24;

        if (calculatedChecksum != bufChecksum) {
            resList.add(new HeaderScanResult(file.getFilePointer() + pos, allocSize, false, "Checksum mismatch", version));
            return;
        }

        long endPos = file.getFilePointer() - size + pos + allocSize;
        if (endPos >= file.length()) {
            resList.add(new HeaderScanResult(file.getFilePointer() + pos, allocSize, false, "Regions partially lays outside the file", version));
            return;
        }
        resList.add(new HeaderScanResult(file.getFilePointer() + pos, allocSize, true, null, version));
    }

    private void scanForHeaders(byte[] data, int size) throws IOException {
        int maxHeaderSize = Math.max(META_HEADER.length(), DATA_HEADER.length());
        for (int i = 0; i < size - maxHeaderSize; i++) {
            if (data[i + 15] == META_HEADER_SIGNATURE[15] && data[i + 16] == META_HEADER_SIGNATURE[16] && data[i + 17] == META_HEADER_SIGNATURE[17] && data[i + 18] == META_HEADER_SIGNATURE[18]) {
                examineHeader(data, size, i);
            } else if (data[i + 15] == DATA_HEADER_SIGNATURE[15] && data[i + 16] == DATA_HEADER_SIGNATURE[16] && data[i + 17] == DATA_HEADER_SIGNATURE[17] && data[i + 18] == DATA_HEADER_SIGNATURE[18]) {
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
                file.seek(file.getFilePointer() - HEADER_SIZE * 2);
            }
        }
    }

    public static FileScanResult scan(RandomAccessFile file) throws IOException {
        FlightRecFileScanner scanner = new FlightRecFileScanner(file);
        scanner.doScan();
        return scanner.scanRes;
    }
}
