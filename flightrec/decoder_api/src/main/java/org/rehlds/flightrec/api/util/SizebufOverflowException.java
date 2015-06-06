package org.rehlds.flightrec.api.util;

public class SizebufOverflowException extends RuntimeException {
    public final String sizebufName;

    public SizebufOverflowException(String sizebufName) {
        super(sizebufName + " overflowed");
        this.sizebufName = sizebufName;
    }
}
