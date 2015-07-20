package org.rehlds.flightrec.textlogwriter;

import org.rehlds.flightrec.api.*;
import org.rehlds.flightrec.logtree.LogTreeNode;
import org.rehlds.flightrec.logtree.LogTreeNodeComplex;
import org.rehlds.flightrec.logtree.LogTreeNodeLeaf;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class TextLogWriter {
    Writer writer;
    List<DecoderModule> decoderModules;

    int indent;
    HashMap<Integer, String> indents = new HashMap<>();
    Map<FlightrecMessageType, MessageDecoder> decodersByMsgType = new HashMap<>();

    public TextLogWriter(Writer writer, List<DecoderModule> decoderModules) {
        this.writer = writer;
        this.decoderModules = decoderModules;
    }

    MessageDecoder lookupDecoder(FlightrecMessageType msgType) {
        for (DecoderModule dm : decoderModules) {
            MessageDecoder d = dm.lookupDecoder(msgType);
            if (d != null) {
                return d;
            }
        }

        return null;
    }

    MessageDecoder getDecoder(FlightrecMessage message) {
        FlightrecMessageType msgType = message.messageType;
        if (!decodersByMsgType.containsKey(msgType)) {
            decodersByMsgType.put(msgType, lookupDecoder(msgType));
        }

        return decodersByMsgType.get(msgType);
    }

    DecodedExtraData tryDecode(FlightrecMessage message) {
        MessageDecoder decoder = getDecoder(message);
        if (decoder == null) {
            return null;
        }
        return decoder.decode(message);
    }

    String escapeString(String s) {
        return s.replace("\"", "\\\"").replace("'", "\\'");
    }

    String generateIndent() {
        String res = indents.get(indent);
        if (res != null) {
            return res;
        }

        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < indent; i++) {
            sb.append("  ");
        }

        res = sb.toString();
        indents.put(indent, res);
        return res;
    }

    void writeExtraData(StringBuilder sb, DecodedExtraData extraData) {
        boolean first = true;
        for (ImmutablePair<String, String> kv : extraData.data) {
            if (first) {
                first = false;
            } else {
                sb.append(", ");
            }
            sb.append(kv.first).append(": '").append(escapeString(kv.second)).append("'");
        }
    }

    String prepareMessageText(FlightrecMessage msg) {
        StringBuilder sb = new StringBuilder();
        sb.append(generateIndent());

        switch (msg.entranceKind) {
            case ENTRANCE_ENTER:
                sb.append(">>");
                break;

            case ENTRANCE_LEAVE:
                sb.append("<<");
                break;

            case ENTRANCE_UNUSED:
                sb.append("--");
                break;

            default:
                throw new RuntimeException("Invalid entrance kind " + msg.entranceKind);
        }

        FlightrecMessageType msgType = msg.messageType;
        sb.append(" ").append(msgType.module).append(".").append(msgType.message).append(":").append(msgType.version).append(" ");

        DecodedExtraData extraData = tryDecode(msg);
        if (extraData != null) {
            writeExtraData(sb, extraData);
        } else {
            sb.append("undecoded[");
            boolean firstByte = true;
            for (int i = msg.rawDataPos; i < msg.rawDataLen + msg.rawDataPos; i++) {
                if (firstByte) {
                    firstByte = false;
                } else {
                    sb.append(" ");
                }
                sb.append(String.format("%02X", msg.rawData[i] & 0xFF));
            }
            sb.append("]");
        }

        sb.append("\n");
        return sb.toString();
    }

    void writeMessage(FlightrecMessage msg) throws IOException {
        String text = prepareMessageText(msg);
        writer.write(text);
    }

    void writeLeafNode(LogTreeNodeLeaf node) throws IOException {
        writeMessage(node.msg);
    }

    void writeComplexNode(LogTreeNodeComplex node) throws IOException {
        if (node.enterMsg != null) {
            writeMessage(node.enterMsg);
        } else {
            writer.write(generateIndent() + ">> [Unknown]");
        }

        indent++;
        writeNodes(node.getChildren());
        indent--;

        if (node.leaveMsg != null) {
            writeMessage(node.leaveMsg);
        } else {
            writer.write(generateIndent() + "<< [Unknown]");
        }
    }

    void writeNodes(List<LogTreeNode> nodes) throws IOException {
        for (LogTreeNode node : nodes) {
            if (node instanceof LogTreeNodeComplex) {
                writeComplexNode((LogTreeNodeComplex) node);
            } else if (node instanceof LogTreeNodeLeaf) {
                writeLeafNode((LogTreeNodeLeaf) node);
            } else {
                throw new RuntimeException("Invalid node class " + node.getClass().getName());
            }
        }
    }


    public static void decodeAndWrite(LogTreeNodeComplex rootNode, File outFile, List<DecoderModule> decoderModules) {
        try (FileWriter fw = new FileWriter(outFile)) {
            TextLogWriter logWriter = new TextLogWriter(fw, decoderModules);
            logWriter.writeNodes(rootNode.getChildren());
        } catch (IOException e) {
            throw new RuntimeException("Failed to open/write file '" + outFile + "': " + e.getMessage(), e);
        }
    }
}
