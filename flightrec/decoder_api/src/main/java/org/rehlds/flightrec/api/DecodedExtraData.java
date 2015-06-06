package org.rehlds.flightrec.api;

public class DecodedExtraData {
    public ImmutablePair<String, String>[] data;

    public DecodedExtraData(ImmutablePair<String, String>[] data) {
        this.data = data;
    }

    public static final DecodedExtraData EMPTY = new DecodedExtraData(new ImmutablePair[0]);

    public static DecodedExtraData create(String... args) {
        if ((args.length % 2) == 1) {
            throw new RuntimeException("DecodedExtraData.create: number of arguments must be even");
        }

        int numPairs = args.length / 2;
        DecodedExtraData res = new DecodedExtraData(new ImmutablePair[numPairs]);

        for (int i = 0; i < numPairs; i++) {
            res.data[i] = new ImmutablePair<>(args[i * 2], args[i * 2 + 1]);
        }

        return res;
    }


}
