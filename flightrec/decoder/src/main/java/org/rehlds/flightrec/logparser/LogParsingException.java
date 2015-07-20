package org.rehlds.flightrec.logparser;

public class LogParsingException extends RuntimeException {

    public LogParsingException(String message) {
        super(message);
    }

    public LogParsingException(String message, Throwable cause) {
        super(message, cause);
    }
}
