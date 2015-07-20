package org.rehlds.flightrec.decoders.rehlds;

import org.rehlds.flightrec.api.DecodedExtraData;
import org.rehlds.flightrec.api.FlightrecMessage;
import org.rehlds.flightrec.api.FlightrecMessageType;
import org.rehlds.flightrec.api.MessageDecoder;
import org.rehlds.flightrec.api.util.UtilSizeBuf;

public class LogV1Decoder implements MessageDecoder {
    @Override
    public FlightrecMessageType getMessageType() {
        return new FlightrecMessageType("rehlds", "Log", 1, false);
    }

    @Override
    public DecodedExtraData decode(FlightrecMessage msg) {
        UtilSizeBuf sb = msg.getDataSizebuf();
        String prefix = sb.readString();
        String message = sb.readString();
        return DecodedExtraData.create("prefix", prefix, "message", message);
    }
}
