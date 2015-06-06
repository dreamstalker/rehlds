package org.rehlds.flightrec.logparser;

import org.rehlds.flightrec.api.EntranceKind;
import org.rehlds.flightrec.api.FlightrecMessage;
import org.rehlds.flightrec.api.FlightrecMessageDef;
import org.rehlds.flightrec.api.FlightrecMessageType;
import org.rehlds.flightrec.filescan.HeaderScanResult;
import org.rehlds.flightrec.api.util.UtilByteBuffer;
import org.rehlds.flightrec.api.util.UtilSizeBuf;
import static org.rehlds.flightrec.Consts.*;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.*;

public class FlightLogParser {
    UtilByteBuffer metaRegion;
    UtilByteBuffer dataRegion;

    MetaHeader metaHeader;
    RecorderState recorderState;
    DataHeader dataHeader;

    Map<Integer, FlightrecMessageType> msgTypes = new HashMap<>();
    List<FlightrecMessage> messages = new ArrayList<>();

    void parseMessageDefinition(UtilSizeBuf sbuf) {
        int msgId = sbuf.readUInt16();
        String module = sbuf.readString();
        String messageName = sbuf.readString();
        long msgVersion = sbuf.readUInt32();
        boolean inOut = sbuf.readBool();

        FlightrecMessageDef msgDef = new FlightrecMessageDef(module, messageName, msgVersion, inOut, msgId);

        if (msgTypes.containsKey(msgId)) {
            System.out.println("Duplicate message id: " + msgTypes.get(msgId) + " and " + msgDef);
        }

        msgTypes.put(msgId, msgDef.msgType);
    }

    void parseMetaRegion() {
        metaHeader = new MetaHeader();
        UtilSizeBuf metaSBuf = new UtilSizeBuf("meta region", metaRegion);
        metaSBuf.skip(META_HEADER_SIG_BYTES.length); //skip signature
        metaSBuf.readInt32(); //version
        metaSBuf.readInt32(); //allocSize
        metaSBuf.readInt32(); //checksum
        metaHeader.numDefinitions = metaSBuf.readInt32();
        metaHeader.metaRegionPos = metaSBuf.readInt32();

        recorderState = new RecorderState();
        recorderState.wpos = metaSBuf.readInt32();
        recorderState.lastMsgBeginPos = metaSBuf.readInt32();
        recorderState.curMessage = metaSBuf.readUInt16();

        metaSBuf = new UtilSizeBuf("meta region defs", metaRegion, META_HEADER_SIZE, metaHeader.metaRegionPos);
        for (int i = 0; i < metaHeader.numDefinitions; i++) {
            int defKind = metaSBuf.readUInt8();
            switch (defKind) {
                case 1: //MRT_MESSAGE_DEF
                    parseMessageDefinition(metaSBuf);
                    break;

                default:
                    throw new RuntimeException("Invalid meta definition type" + defKind);
            }
        }

        dataHeader = new DataHeader();
        dataHeader.prevItrLastPos = dataRegion.readInt32(DATA_HEADER_SIG_BYTES.length + 12);
    }

    public FlightLogParser(UtilByteBuffer metaRegion, UtilByteBuffer dataRegion) {
        this.metaRegion = metaRegion;
        this.dataRegion = dataRegion;
    }

    void doParseMessage(UtilSizeBuf msg) {
        int opc = msg.readUInt16();
        boolean entrance = (0 != (opc & 0x8000));
        opc &= 0x7FFF;

        FlightrecMessageType msgType = msgTypes.get(opc);
        if (msgType == null) {
            throw new RuntimeException("Invalid message opcode @" + Long.toHexString(msg.getAbsoluteCurrentPos() - 2) + ": " + opc);
        }

        EntranceKind entranceKind;
        if (msgType.inout) {
            entranceKind = entrance ? EntranceKind.ENTRANCE_ENTER : EntranceKind.ENTRANCE_LEAVE;
        } else {
            entranceKind = EntranceKind.ENTRANCE_UNUSED;
        }

        FlightrecMessage flMsg = new FlightrecMessage(msgType, entranceKind, msg.getBuffer(), msg.getAbsoluteCurrentPos(), msg.getMaxSize() - 2);
        messages.add(flMsg);
    }

    void parseMessage(UtilSizeBuf msg) {
        int startPos = msg.getStartPos();
        try {
            doParseMessage(msg);
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error while parsing message @" + startPos);
        }
    }

    List<FlightrecMessage> parse() {
        parseMetaRegion();

        UtilByteBuffer flightData = dataRegion.cutLeft(DATA_HEADER_SIZE);

        boolean flippedToEnd = false;

        /*
            Each message has following layout:
            Opcode [2 bytes]
            Data [0+ bytes]
            Length of opcode + data [2 bytes]
         */
        int curMsgPos = (recorderState.curMessage == 0) ? recorderState.wpos : recorderState.lastMsgBeginPos;
        curMsgPos -= 2; //position of the Length field of the message

        UtilSizeBuf msg = new UtilSizeBuf("flightrec_message", flightData, 0, 0);

        while (true) {
            if (flippedToEnd && curMsgPos <= recorderState.wpos)
                break;

            if (curMsgPos <= 0) { //move read pointer to the end of the data region
                if (dataHeader.prevItrLastPos == -1) //wpos never reached end of the region
                    break;

                curMsgPos = dataHeader.prevItrLastPos - 2;
                flippedToEnd = true;
                continue;
            }

            int msgLen = flightData.readUInt16(curMsgPos);
            int msgStartPos = curMsgPos - msgLen;
            if (msgStartPos < 0) {
                throw new RuntimeException("Corrupted data region; read msgLen=" + msgLen + " at " + curMsgPos + ", but it is too large (startPos < 0)");
            }

            if (flippedToEnd && msgStartPos < recorderState.wpos) {
                break;
            }

            msg.init(msgStartPos, msgLen);
            parseMessage(msg);

            curMsgPos = msgStartPos - 2;
        }

        return messages;
    }

    public static List<FlightrecMessage> doParse(RandomAccessFile f, HeaderScanResult metaHeader, HeaderScanResult dataHeader) throws IOException {
        //read regions to byte buffers
        f.seek(metaHeader.pos);
        byte[] metaRegionData = new byte[metaHeader.len];
        f.readFully(metaRegionData);

        f.seek(dataHeader.pos);
        byte[] dataRegionData = new byte[dataHeader.len];
        f.readFully(dataRegionData);

        UtilByteBuffer metaRegion = new UtilByteBuffer(metaRegionData);
        UtilByteBuffer dataRegion = new UtilByteBuffer(dataRegionData);

        List<FlightrecMessage> res = new FlightLogParser(metaRegion, dataRegion).parse();
        Collections.reverse(res);
        return res;
    }
}

