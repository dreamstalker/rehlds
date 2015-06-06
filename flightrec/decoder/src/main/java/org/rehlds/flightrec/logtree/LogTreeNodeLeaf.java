package org.rehlds.flightrec.logtree;

import org.rehlds.flightrec.api.FlightrecMessage;

import java.util.Collections;
import java.util.List;

public class LogTreeNodeLeaf extends LogTreeNode {
    public FlightrecMessage msg;

    public LogTreeNodeLeaf(LogTreeNodeComplex parent, FlightrecMessage msg) {
        super(parent);
        this.msg = msg;
    }

    @Override
    List<? extends LogTreeNode> getChildren() {
        return Collections.emptyList();
    }
}
