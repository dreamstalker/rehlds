package com.google.cloud;

import org.junit.Test;

import static org.junit.Assert.*;

public class Crc32cTest {

    static class TestData {
        public String src;
        public long hash;

        TestData(String src, long hash) {
            this.src = src;
            this.hash = hash;
        }
    }

    @Test
    public void testCrc32c() {
        TestData testData[] = {
                new TestData("a", 0x93AD1061L),
                new TestData("ab", 0x13C35EE4L),
                new TestData("abc", 0x562F9CCDL),
                new TestData("abcd", 0xDAAF41F6L),
                new TestData("abcde", 0x8122A0A2L),
                new TestData("abcdef", 0x0496937BL),
                new TestData("abcdefg", 0x5D199E2CL),
                new TestData("abcdefgh", 0x86BC933DL),
                new TestData("abcdefghi", 0x9639F15FL),
                new TestData("abcdefghij", 0x0584645CL),
        };

        for (TestData t : testData) {
            Crc32c crc32c = new Crc32c();
            crc32c.update(t.src.getBytes());
            long cksum = crc32c.getValue();

            assertEquals(t.hash, cksum);
        }
    }

}