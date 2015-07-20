package org.rehlds.flightrec.logparser;

public class RecorderState {
    public int wpos;
    public int lastMsgBeginPos;
    public int curMessage;

    public RecorderState(int wpos, int lastMsgBeginPos, int curMessage) {
        this.wpos = wpos;
        this.lastMsgBeginPos = lastMsgBeginPos;
        this.curMessage = curMessage;
    }

    public RecorderState() {
    }
}
