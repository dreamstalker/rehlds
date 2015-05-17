package org.rehlds.flightrec.filescan;

public class HeaderScanResult {
    public long pos;
    public int len;
    public boolean valid;
    public String error;
    public int version;

    public HeaderScanResult() {
    }

    public HeaderScanResult(long pos, int len, boolean valid, String error, int version) {
        this.pos = pos;
        this.len = len;
        this.valid = valid;
        this.error = error;
        this.version = version;
    }
}
