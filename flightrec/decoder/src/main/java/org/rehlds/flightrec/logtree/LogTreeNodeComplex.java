package org.rehlds.flightrec.logtree;

import org.rehlds.flightrec.api.FlightrecMessage;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class LogTreeNodeComplex extends LogTreeNode {
    public FlightrecMessage enterMsg;
    public FlightrecMessage leaveMsg;

    public LogTreeNodeComplex(LogTreeNodeComplex parent, FlightrecMessage enterMsg, FlightrecMessage leaveMsg) {
        super(parent);
        this.enterMsg = enterMsg;
        this.leaveMsg = leaveMsg;
    }

    List<LogTreeNode> children = Collections.emptyList();



    @Override
    public List<LogTreeNode> getChildren() {
        return children;
    }

    public void addChild(LogTreeNode node) {
        if (children.isEmpty()) {
            children = new ArrayList<>();
        }

        children.add(node);
    }

}
