package org.rehlds.flightrec.util;

import org.junit.Test;
import org.rehlds.flightrec.api.util.UtilByteBuffer;

import static org.junit.Assert.*;

public class UtilByteBufferTest {

    @Test
    public void testReadUInt8() throws Exception {
        byte data[] = { 0x10, 0x00, 0x7F, (byte)0x80, (byte)0xFF };
        UtilByteBuffer bb = new UtilByteBuffer(data);

        assertEquals(0x10, bb.readUInt8(0));
        assertEquals(0x00, bb.readUInt8(1));
        assertEquals(0x7F, bb.readUInt8(2));
        assertEquals(0x80, bb.readUInt8(3));
        assertEquals(0xFF, bb.readUInt8(4));
    }

    @Test
    public void testReadUInt16() throws Exception {
        byte data[] = { 0x10, 0x00, 0x7F, (byte)0x80, (byte)0xFF, 0x00 };
        UtilByteBuffer bb = new UtilByteBuffer(data);

        assertEquals(0x10, bb.readUInt16(0));
        assertEquals(0x7F00, bb.readUInt16(1));
        assertEquals(0x807F, bb.readUInt16(2));
        assertEquals(0xFF80, bb.readUInt16(3));
        assertEquals(0x00FF, bb.readUInt16(4));
    }

    @Test
    public void testReadUInt32() throws Exception {
        byte data[] = { 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, (byte)0xFF, 0x00, 0x00, 0x00 };
        UtilByteBuffer bb = new UtilByteBuffer(data);
        assertEquals(0x10000000, bb.readUInt32(0));
        assertEquals(0x100000, bb.readUInt32(1));
        assertEquals(0x1000, bb.readUInt32(2));
        assertEquals(0x10, bb.readUInt32(3));
        assertEquals(0x0, bb.readUInt32(4));
        assertEquals(0x7F000000, bb.readUInt32(5));
        assertEquals(0x7F0000, bb.readUInt32(6));
        assertEquals(0x7F00, bb.readUInt32(7));
        assertEquals(0x7F, bb.readUInt32(8));
        assertEquals(0xFF000000L, bb.readUInt32(9));
        assertEquals(0xFF0000, bb.readUInt32(10));
        assertEquals(0xFF00, bb.readUInt32(11));
        assertEquals(0xFF, bb.readUInt32(12));
    }

    @Test
    public void testReadInt32() throws Exception {
        byte data[] = { 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, (byte)0xFF, 0x00, 0x00, 0x00 };
        UtilByteBuffer bb = new UtilByteBuffer(data);
        assertEquals(0x10000000, bb.readInt32(0));
        assertEquals(0x100000, bb.readInt32(1));
        assertEquals(0x1000, bb.readInt32(2));
        assertEquals(0x10, bb.readInt32(3));

        assertEquals(0x0, bb.readInt32(4));

        assertEquals(0x7F000000, bb.readInt32(5));
        assertEquals(0x7F0000, bb.readInt32(6));
        assertEquals(0x7F00, bb.readInt32(7));
        assertEquals(0x7F, bb.readInt32(8));

        assertEquals((int)0xFF000000, bb.readInt32(9));
        assertEquals(0xFF0000, bb.readInt32(10));
        assertEquals(0xFF00, bb.readInt32(11));
        assertEquals(0xFF, bb.readInt32(12));
    }

    @Test
    public void testReadInt64() throws Exception {
        byte data[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, (byte)0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        UtilByteBuffer bb = new UtilByteBuffer(data);
        assertEquals(0x1000000000000000L, bb.readInt64(0));
        assertEquals(0x10000000000000L, bb.readInt64(1));
        assertEquals(0x100000000000L, bb.readInt64(2));
        assertEquals(0x1000000000L, bb.readInt64(3));
        assertEquals(0x10000000L, bb.readInt64(4));
        assertEquals(0x100000L, bb.readInt64(5));
        assertEquals(0x1000L, bb.readInt64(6));
        assertEquals(0x10L, bb.readInt64(7));

        assertEquals(0x00L, bb.readInt64(8));

        assertEquals(0x7F00000000000000L, bb.readInt64(9));
        assertEquals(0x7F000000000000L, bb.readInt64(10));
        assertEquals(0x7F0000000000L, bb.readInt64(11));
        assertEquals(0x7F00000000L, bb.readInt64(12));
        assertEquals(0x7F000000L, bb.readInt64(13));
        assertEquals(0x7F0000L, bb.readInt64(14));
        assertEquals(0x7F00L, bb.readInt64(15));
        assertEquals(0x7FL, bb.readInt64(16));

        assertEquals(0xFF00000000000000L, bb.readInt64(17));
        assertEquals(0xFF000000000000L, bb.readInt64(18));
        assertEquals(0xFF0000000000L, bb.readInt64(19));
        assertEquals(0xFF00000000L, bb.readInt64(20));
        assertEquals(0xFF000000L, bb.readInt64(21));
        assertEquals(0xFF0000L, bb.readInt64(22));
        assertEquals(0xFF00L, bb.readInt64(23));
        assertEquals(0xFFL, bb.readInt64(24));
    }

    @Test
    public void testReadString() throws Exception {
        byte data[] = { 0x00, 0x41, 0x00, 0x41, 0x42, 0x00, 0x50, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x69, 0x74, (byte)0xC3, (byte)0xA9, 0x00, 0x41, 0x42 };
        UtilByteBuffer bb = new UtilByteBuffer(data);

        assertEquals("", bb.readString(0));
        assertEquals("A", bb.readString(1));
        assertEquals("", bb.readString(2));
        assertEquals("AB", bb.readString(3));
        assertEquals("Publicité", bb.readString(6));
        assertNull(bb.readString(17));

        assertEquals("Public", bb.readString(6, 6, false));
        assertNull(bb.readString(6, 6, true));

        assertEquals("AB", bb.readString(17, 2, false));
        assertNull(bb.readString(17, 2, true));
    }
}