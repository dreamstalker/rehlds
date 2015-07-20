package org.rehlds.flightrec.api.util;

public class UtilSizeBuf {
    String name;
    UtilByteBuffer buf;
    int startPos;
    int maxSize;
    int curPos;

    public UtilSizeBuf(String name, UtilByteBuffer buf, int startPos, int maxSize) {
        this.name = name;
        this.buf = buf;
        this.startPos = startPos;
        this.maxSize = maxSize;
        curPos = 0;
    }

    public UtilSizeBuf(String name, UtilByteBuffer buf) {
        this(name, buf, 0, buf.getDataLength());
    }

    public void init(int startPos, int maxSize) {
        this.startPos = startPos;
        this.maxSize = maxSize;
        curPos = 0;
    }

    public void reset() {
        this.curPos = 0;
    }

    public int tell() {
        return curPos;
    }

    public int getAbsoluteCurrentPos() {
        return curPos + startPos;
    }

    public void skip(int count) {
        if (curPos + count > maxSize) {
            curPos = maxSize;
            throw new SizebufOverflowException(name);
        }

        curPos += count;
    }

    public int readUInt8() {
        if (curPos + 1 > maxSize) {
            curPos = maxSize;
            throw new SizebufOverflowException(name);
        }

        int pos = curPos;
        curPos++;
        return buf.readUInt8(pos + this.startPos);
    }

    public boolean readBool() {
        if (curPos + 1 > maxSize) {
            curPos = maxSize;
            throw new SizebufOverflowException(name);
        }

        int pos = curPos;
        curPos++;
        return buf.readBool(pos + this.startPos);
    }

    public int readUInt16() {
        if (curPos + 2 > maxSize) {
            curPos = maxSize;
            throw new SizebufOverflowException(name);
        }

        int pos = curPos;
        curPos += 2;
        return buf.readUInt16(pos + this.startPos);
    }

    public long readUInt32() {
        if (curPos + 4 > maxSize) {
            curPos = maxSize;
            throw new SizebufOverflowException(name);
        }

        int pos = curPos;
        curPos += 4;
        return buf.readUInt32(pos + this.startPos);
    }

    public int readInt32() {
        if (curPos + 4 > maxSize) {
            curPos = maxSize;
            throw new SizebufOverflowException(name);
        }

        int pos = curPos;
        curPos += 4;
        return buf.readInt32(pos + this.startPos);
    }

    public long readInt64() {
        if (curPos + 8 > maxSize) {
            curPos = maxSize;
            throw new SizebufOverflowException(name);
        }

        int pos = curPos;
        curPos += 8;
        return buf.readInt64(pos + this.startPos);
    }

    public double readDouble() {
        if (curPos + 8 > maxSize) {
            curPos = maxSize;
            throw new SizebufOverflowException(name);
        }

        int pos = curPos;
        curPos += 8;
        return buf.readDouble(pos + this.startPos);
    }

    public float readFloat() {
        if (curPos + 4 > maxSize) {
            curPos = maxSize;
            throw new SizebufOverflowException(name);
        }

        int pos = curPos;
        curPos += 4;
        return buf.readFloat(pos + this.startPos);
    }

    public String readString() {
        String s = buf.readString(curPos + this.startPos);
        if (s == null) {
            curPos = maxSize;
            throw new SizebufOverflowException(name);
        }

        curPos += s.getBytes(Globals.UTF8).length + 1;
        return s;
    }

    public int getStartPos() {
        return startPos;
    }

    public int getMaxSize() {
        return maxSize;
    }

    public byte[] getBuffer() {
        return buf.getData();
    }
}
