package org.rehlds.flightrec.api;

public interface MessageDecoder {
    FlightrecMessageType getMessageType();
    DecodedExtraData decode(FlightrecMessage msg);
}
