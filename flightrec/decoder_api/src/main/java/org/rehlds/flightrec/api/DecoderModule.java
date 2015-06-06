package org.rehlds.flightrec.api;

public interface DecoderModule {
    public MessageDecoder lookupDecoder(FlightrecMessageType msgType);
    public String getDescription();
    public String getVersion();
}
