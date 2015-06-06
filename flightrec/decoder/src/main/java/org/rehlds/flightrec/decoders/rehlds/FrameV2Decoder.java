package org.rehlds.flightrec.decoders.rehlds;

import org.rehlds.flightrec.api.DecodedExtraData;
import org.rehlds.flightrec.api.FlightrecMessage;
import org.rehlds.flightrec.api.FlightrecMessageType;
import org.rehlds.flightrec.api.MessageDecoder;
import org.rehlds.flightrec.api.util.UtilSizeBuf;

public class FrameV2Decoder implements MessageDecoder {
    @Override
    public FlightrecMessageType getMessageType() {
        return new FlightrecMessageType("rehlds", "Frame", 2, true);
    }

    DecodedExtraData decodeStart(UtilSizeBuf sb) {
        long frameId = sb.readInt64();
        double startTime = sb.readDouble();
        return DecodedExtraData.create("frameId", "" + frameId, "startTime", "" + startTime);
    }

    DecodedExtraData decodeEnd(UtilSizeBuf sb) {
        long frameId = sb.readInt64();
        return DecodedExtraData.create("frameId", "" + frameId);
    }

    @Override
    public DecodedExtraData decode(FlightrecMessage msg) {
        UtilSizeBuf sb = msg.getDataSizebuf();
        return msg.isEnterMessage() ? decodeStart(sb) : decodeEnd(sb);
    }

}
