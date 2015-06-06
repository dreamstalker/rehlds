package org.rehlds.flightrec.api;

import org.rehlds.flightrec.api.util.UtilByteBuffer;
import org.rehlds.flightrec.api.util.UtilSizeBuf;

public class FlightrecMessage {
    public final FlightrecMessageType messageType;
    public final EntranceKind entranceKind;

    public final byte[] rawData;
    public final int rawDataPos;
    public final int rawDataLen;

    DecodedExtraData decodedData;

    public FlightrecMessage(FlightrecMessageType messageType, EntranceKind entranceKind, byte[] rawData, int rawDataOffset, int rawDataLen) {
        this.messageType = messageType;
        this.entranceKind = entranceKind;
        this.rawData = rawData;
        this.rawDataPos = rawDataOffset;
        this.rawDataLen = rawDataLen;
    }

    public UtilSizeBuf getDataSizebuf() {
        return new UtilSizeBuf("msg: '" + messageType + "' @" + rawDataPos, new UtilByteBuffer(rawData), rawDataPos, rawDataLen);
    }

    public boolean isEnterMessage() {
        return (entranceKind == EntranceKind.ENTRANCE_ENTER);
    }

    public boolean isLeaveMessage() {
        return (entranceKind == EntranceKind.ENTRANCE_LEAVE);
    }
}
