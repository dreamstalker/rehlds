package org.rehlds.flightrec.logparser;

public class MetaHeader {
    public int numDefinitions;
    public int metaRegionPos;

    public MetaHeader(int numMessages, int metaRegionPos) {
        this.numDefinitions = numMessages;
        this.metaRegionPos = metaRegionPos;
    }

    public MetaHeader() {
    }
}
