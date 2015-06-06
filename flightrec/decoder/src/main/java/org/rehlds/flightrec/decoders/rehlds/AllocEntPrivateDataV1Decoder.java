package org.rehlds.flightrec.decoders.rehlds;


import org.rehlds.flightrec.api.DecodedExtraData;
import org.rehlds.flightrec.api.FlightrecMessage;
import org.rehlds.flightrec.api.FlightrecMessageType;
import org.rehlds.flightrec.api.MessageDecoder;
import org.rehlds.flightrec.api.util.UtilSizeBuf;

public class AllocEntPrivateDataV1Decoder implements MessageDecoder {
    @Override
    public FlightrecMessageType getMessageType() {
        return new FlightrecMessageType("rehlds", "AllocEntPrivateData", 1, false);
    }

    @Override
    public DecodedExtraData decode(FlightrecMessage msg) {
        UtilSizeBuf sb = msg.getDataSizebuf();
        long ptr = sb.readUInt32();
        return DecodedExtraData.create("pPrivData", "0x" + Long.toHexString(ptr));
    }
}
