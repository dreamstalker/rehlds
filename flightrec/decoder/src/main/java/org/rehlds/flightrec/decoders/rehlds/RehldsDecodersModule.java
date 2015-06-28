package org.rehlds.flightrec.decoders.rehlds;

import org.rehlds.flightrec.api.SimpleDecoderModule;

public class RehldsDecodersModule extends SimpleDecoderModule {

    public RehldsDecodersModule() {
        super("Rehlds decoders (built-in)", "0.2");
        registerDecoder(new FrameV1Decoder());
        registerDecoder(new FreeEntPrivateDataV1Decoder());
        registerDecoder(new AllocEntPrivateDataV1Decoder());

        registerDecoder(new FrameV2Decoder());

        registerDecoder(new LogV1Decoder());
        registerDecoder(new AllocEntPrivateDataV2Decoder());
    }
}
