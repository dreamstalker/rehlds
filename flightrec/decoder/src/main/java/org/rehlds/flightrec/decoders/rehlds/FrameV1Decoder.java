package org.rehlds.flightrec.decoders.rehlds;

import org.rehlds.flightrec.api.DecodedExtraData;
import org.rehlds.flightrec.api.FlightrecMessage;
import org.rehlds.flightrec.api.FlightrecMessageType;
import org.rehlds.flightrec.api.MessageDecoder;
import org.rehlds.flightrec.api.util.UtilSizeBuf;

public class FrameV1Decoder implements MessageDecoder {
    @Override
    public FlightrecMessageType getMessageType() {
        return new FlightrecMessageType("rehlds", "Frame", 1, true);
    }

    DecodedExtraData decodeStart(UtilSizeBuf sb) {
        double startTime = sb.readDouble();
        return DecodedExtraData.create("startTime", "" + startTime);
    }

    DecodedExtraData decodeEnd(UtilSizeBuf sb) {
        return DecodedExtraData.EMPTY;
    }

    @Override
    public DecodedExtraData decode(FlightrecMessage msg) {
        UtilSizeBuf sb = msg.getDataSizebuf();
        return msg.isEnterMessage() ? decodeStart(sb) : decodeEnd(sb);
    }

}
