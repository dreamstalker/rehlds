package org.rehlds.flightrec.api;

public class FlightrecMessageDef {
    public final FlightrecMessageType msgType;
    public final int opcode;

    public FlightrecMessageDef(String module, String message, long version, boolean inout, int opcode) {
        msgType = new FlightrecMessageType(module, message, version, inout);
        this.opcode = opcode;
    }

    @Override
    public String toString() {
        return "FlightrecMessageDef{" +
                "module='" + msgType.module + '\'' +
                ", message='" + msgType.message + '\'' +
                ", version=" + msgType.version +
                ", inout=" + msgType.inout +
                ", opcode=" + opcode +
                '}';
    }
}
