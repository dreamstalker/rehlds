package org.rehlds.flightrec.api;

public class FlightrecMessageType {
    public final String module;
    public final String message;
    public final long version;
    public final boolean inout;

    public FlightrecMessageType(String module, String message, long version, boolean inout) {
        this.module = module;
        this.message = message;
        this.version = version;
        this.inout = inout;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        FlightrecMessageType that = (FlightrecMessageType) o;

        if (inout != that.inout) return false;
        if (version != that.version) return false;
        if (!message.equals(that.message)) return false;
        if (!module.equals(that.module)) return false;

        return true;
    }

    @Override
    public int hashCode() {
        int result = module.hashCode();
        result = 31 * result + message.hashCode();
        result = 31 * result + (int) (version ^ (version >>> 32));
        result = 31 * result + (inout ? 1 : 0);
        return result;
    }

    @Override
    public String toString() {
        return "FlightrecMessageType{" +
                "module='" + module + '\'' +
                ", message='" + message + '\'' +
                ", version=" + version +
                ", inout=" + inout +
                '}';
    }
}
