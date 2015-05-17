package org.rehlds.flightrec.api;

public class FlightrecMessage {
    String type;
    int version;
    byte[] rawData;
    EntranceKind entranceKind;

    DecodedExtraData decodedData;
}
