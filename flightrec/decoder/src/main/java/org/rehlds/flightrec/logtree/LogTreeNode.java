package org.rehlds.flightrec.logtree;

import java.util.List;

public abstract class LogTreeNode {
    LogTreeNodeComplex parent;

    protected LogTreeNode(LogTreeNodeComplex parent) {
        this.parent = parent;
    }

    abstract List<? extends LogTreeNode> getChildren();

    LogTreeNodeComplex getParent() {
        return parent;
    }

    public void setParent(LogTreeNodeComplex parent) {
        this.parent = parent;
    }
}
