package org.rehlds.flightrec.api.util;

public class UtilByteBuffer {
    byte data[];

    public UtilByteBuffer(byte[] data) {
        this.data = data;
    }

    public byte[] getData() {
        return data;
    }

    public UtilByteBuffer cutLeft(int newStart) {
        byte[] newData = new byte[data.length - newStart];
        System.arraycopy(data, newStart, newData, 0, data.length - newStart);
        return new UtilByteBuffer(newData);
    }

    public int getDataLength() {
        return data.length;
    }

    public int readUInt8(int pos) {
        return data[pos] & 0xFF;
    }

    public boolean readBool(int pos) {
        return data[pos] != 0;
    }

    public int readUInt16(int pos) {
        return (data[pos] & 0xFF) | ((data[pos + 1] & 0xFF) << 8);
    }

    public long readUInt32(int pos) {
        return (data[pos] & 0xFF) | ((data[pos + 1] & 0xFF) << 8) | ((data[pos + 2] & 0xFF) << 16) | ((long)(data[pos + 3] & 0xFF) << 24);
    }

    public int readInt32(int pos) {
        return (data[pos] & 0xFF) | ((data[pos + 1] & 0xFF) << 8) | ((data[pos + 2] & 0xFF) << 16) | ((data[pos + 3] & 0xFF) << 24);
    }

    public long readInt64(int pos) {
        long lowBits = readUInt32(pos);
        long highBits = readUInt32(pos + 4);

        return lowBits | (highBits << 32);
    }

    public double readDouble(int pos) {
        long bits = readInt64(pos);
        return Double.longBitsToDouble(bits);
    }

    public float readFloat(int pos) {
        int bits = readInt32(pos);
        return Float.intBitsToFloat(bits);
    }

    public String readString(int pos) {
        return readString(pos, data.length - pos, true);
    }

    public String readString(int pos, int maxSize, boolean errorOnMaxSizeHit) {
        int iMax = Math.min(data.length, pos + maxSize);
        for (int i = pos; i < iMax; i++) {
            if (data[i] == 0) {
                return new String(data, pos, i - pos, Globals.UTF8);
            }
        }

        if (errorOnMaxSizeHit) {
            return null;
        }

        return new String(data, pos, iMax - pos, Globals.UTF8);
    }
}
