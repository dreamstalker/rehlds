package org.rehlds.flightrec;

public class Consts {
    public final static String META_HEADER_SIG_STR = "REHLDS_FLIGHTREC_META";
    public final static String DATA_HEADER_SIG_STR = "REHLDS_FLIGHTREC_DATA";

    public static byte[] META_HEADER_SIG_BYTES = (META_HEADER_SIG_STR + META_HEADER_SIG_STR + META_HEADER_SIG_STR + ":").getBytes();
    public static byte[] DATA_HEADER_SIG_BYTES = (DATA_HEADER_SIG_STR + DATA_HEADER_SIG_STR + DATA_HEADER_SIG_STR + ":").getBytes();

    public static int META_HEADER_SIZE = 128;
    public static int DATA_HEADER_SIZE = 128;

    public static int MAX_HEADER_SIZE = Math.max(META_HEADER_SIZE, DATA_HEADER_SIZE);
}
