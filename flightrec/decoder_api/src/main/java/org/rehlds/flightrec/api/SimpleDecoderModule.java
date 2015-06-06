package org.rehlds.flightrec.api;

import java.util.HashMap;
import java.util.Map;

public class SimpleDecoderModule implements DecoderModule {
    Map<FlightrecMessageType, MessageDecoder> decoders = new HashMap<>();

    public final String description;
    public final String version;

    public SimpleDecoderModule(String description, String version) {
        this.description = description;
        this.version = version;
    }

    @Override
    public MessageDecoder lookupDecoder(FlightrecMessageType msgType) {
        return decoders.get(msgType);
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public String getVersion() {
        return version;
    }

    public void registerDecoder(MessageDecoder msgDecoder) {
        decoders.put(msgDecoder.getMessageType(), msgDecoder);
    }
}
