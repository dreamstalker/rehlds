package org.rehlds.flightrec.api;

public class ImmutablePair<T, U> {
    public final T first;
    public final U second;

    public ImmutablePair(T first, U second) {
        this.first = first;
        this.second = second;
    }
}
