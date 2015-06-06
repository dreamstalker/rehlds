package org.rehlds.flightrec.logtree;

import org.rehlds.flightrec.api.FlightrecMessage;
import org.rehlds.flightrec.api.FlightrecMessageType;

import java.util.List;

public class FlightLogTreeBuilder {
    LogTreeNodeComplex rootNode = new LogTreeNodeComplex(null, null, null);
    LogTreeNodeComplex currentNode = rootNode;

    void handleEnterMessage(FlightrecMessage msg) {
        LogTreeNodeComplex n = new LogTreeNodeComplex(currentNode, msg, null);
        currentNode.addChild(n);
        currentNode = n;
    }

    void handleLeaveMessage(FlightrecMessage msg) {
        if (currentNode == rootNode) {
            rootNode = new LogTreeNodeComplex(null, null, msg);
            rootNode.addChild(currentNode);
            currentNode.setParent(rootNode);
            currentNode = rootNode;
            return;
        }

        if (currentNode.enterMsg != null) {
            FlightrecMessageType startType = currentNode.enterMsg.messageType;
            FlightrecMessageType endType = msg.messageType;
            if (!startType.equals(endType)) {
                throw new RuntimeException("Closing message @" + Long.toHexString(msg.rawDataPos) + " has invalid type " + endType + "; expected " + startType);
            }
        }

        currentNode.leaveMsg = msg;
        currentNode = currentNode.parent;
    }

    void handleSimpleMessage(FlightrecMessage msg) {
        LogTreeNodeLeaf leafNode = new LogTreeNodeLeaf(currentNode, msg);
        currentNode.addChild(leafNode);
    }

    void doBuildLogTree(List<FlightrecMessage> messages) {
        for (FlightrecMessage msg : messages) {
            switch (msg.entranceKind) {
                case ENTRANCE_ENTER:
                    handleEnterMessage(msg);
                    break;

                case ENTRANCE_LEAVE:
                    handleLeaveMessage(msg);
                    break;

                case ENTRANCE_UNUSED:
                    handleSimpleMessage(msg);
                    break;

                default:
                    throw new RuntimeException("Invalid exntrance kind");
            }
        }
    }


    public static LogTreeNodeComplex buildTree(List<FlightrecMessage> messages) {
        FlightLogTreeBuilder builder = new FlightLogTreeBuilder();
        builder.doBuildLogTree(messages);
        return builder.rootNode;
    }
}
