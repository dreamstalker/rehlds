package org.rehlds.flightrec.logtree

import org.junit.Test
import org.rehlds.flightrec.api.EntranceKind
import org.rehlds.flightrec.api.FlightrecMessage
import org.rehlds.flightrec.api.FlightrecMessageType

class FlightLogTreeBuilderTest {
    static final FlightrecMessageType hierarchyMsgType1 = new FlightrecMessageType('test', 'hmsg1', 1, true);
    static final FlightrecMessageType hierarchyMsgType2 = new FlightrecMessageType('test', 'hmsg1', 1, true);
    static final FlightrecMessageType flatMsgType1 = new FlightrecMessageType('test', 'flatmsg1', 1, true);
    static final FlightrecMessageType flatMsgType2 = new FlightrecMessageType('test', 'flatmsg2', 1, true);

    static FlightrecMessage enterMsg(FlightrecMessageType type) {
        return new FlightrecMessage(type, EntranceKind.ENTRANCE_ENTER, null, 0, 0);
    }

    static FlightrecMessage leaveMsg(FlightrecMessageType type) {
        return new FlightrecMessage(type, EntranceKind.ENTRANCE_LEAVE, null, 0, 0);
    }

    static FlightrecMessage flatMsg(FlightrecMessageType type) {
        return new FlightrecMessage(type, EntranceKind.ENTRANCE_UNUSED, null, 0, 0);
    }

    @Test
    void 'decode 2 flat messages'() {
        def messages = [flatMsg(flatMsgType1), flatMsg(flatMsgType2)]
        def rootNode = FlightLogTreeBuilder.buildTree(messages)

        assert rootNode.children.size() == 2
        assert rootNode.children[0].msg == messages[0];
        assert rootNode.children[1].msg == messages[1];

        assert rootNode.enterMsg == null
        assert rootNode.leaveMsg == null
    }

    @Test
    void 'decode 2 empty hierarchy msgs'() {
        def messages = [
                enterMsg(hierarchyMsgType1), leaveMsg(hierarchyMsgType1),
                enterMsg(hierarchyMsgType2), leaveMsg(hierarchyMsgType2)
        ]
        def rootNode = FlightLogTreeBuilder.buildTree(messages)

        assert rootNode.children.size() == 2

        rootNode.children[0].enterMsg == messages[0]
        rootNode.children[0].leaveMsg == messages[1]
        assert rootNode.children[0].children.empty

        rootNode.children[1].enterMsg == messages[2]
        rootNode.children[1].leaveMsg == messages[3]
        assert rootNode.children[1].children.empty

        assert rootNode.enterMsg == null
        assert rootNode.leaveMsg == null
    }

    @Test
    void 'decode 2 hierarchy messages with flat payload'() {
        def messages = [
                enterMsg(hierarchyMsgType1), flatMsg(flatMsgType1), leaveMsg(hierarchyMsgType1),
                enterMsg(hierarchyMsgType2), flatMsg(flatMsgType2), leaveMsg(hierarchyMsgType2)
        ]
        def rootNode = FlightLogTreeBuilder.buildTree(messages)

        assert rootNode.children.size() == 2

        rootNode.children[0].enterMsg == messages[0]
        rootNode.children[0].leaveMsg == messages[2]
        assert rootNode.children[0].children.size() == 1
        assert rootNode.children[0].children[0].msg == messages[1]

        rootNode.children[1].enterMsg == messages[3]
        rootNode.children[1].leaveMsg == messages[5]
        assert rootNode.children[1].children.size() == 1
        assert rootNode.children[1].children[0].msg == messages[4]

        assert rootNode.enterMsg == null
        assert rootNode.leaveMsg == null
    }

    @Test
    void 'decode hierarchical message with mixed payload'() {
        def messages = [
                flatMsg(flatMsgType2),
                enterMsg(hierarchyMsgType1),
                    flatMsg(flatMsgType1),
                    enterMsg(hierarchyMsgType2),
                        flatMsg(flatMsgType2),
                    leaveMsg(hierarchyMsgType2),
                    flatMsg(flatMsgType2),
                leaveMsg(hierarchyMsgType1),
                flatMsg(flatMsgType1)
        ]
        def rootNode = FlightLogTreeBuilder.buildTree(messages)

        assert rootNode.children.size() == 3
        assert rootNode.enterMsg == null
        assert rootNode.leaveMsg == null

        assert rootNode.children[0].msg == messages[0]
        assert rootNode.children[2].msg == messages[8]

        assert rootNode.children[1].enterMsg == messages[1]
        assert rootNode.children[1].leaveMsg == messages[7]
        assert rootNode.children[1].children.size() == 3

        assert rootNode.children[1].children[0].msg == messages[2]
        assert rootNode.children[1].children[2].msg == messages[6]

        assert rootNode.children[1].children[1].enterMsg == messages[3]
        assert rootNode.children[1].children[1].leaveMsg == messages[5]
        assert rootNode.children[1].children[1].children.size() == 1

        assert rootNode.children[1].children[1].children[0].msg == messages[4]
    }

    @Test
    void 'decode hierarchical msg with flat payload and missing start'() {
        def messages = [
                flatMsg(flatMsgType1),
                leaveMsg(hierarchyMsgType1),
                flatMsg(flatMsgType2)
        ]
        def rootNode = FlightLogTreeBuilder.buildTree(messages)

        assert rootNode.children.size() == 2
        assert rootNode.enterMsg == null
        assert rootNode.leaveMsg == null

        assert rootNode.children[0].enterMsg == null
        assert rootNode.children[0].leaveMsg == messages[1]
        assert rootNode.children[0].children.size() == 1
        assert rootNode.children[0].children[0].msg == messages[0]

        assert rootNode.children[1].msg == messages[2]
    }

    @Test
    void 'decode empty hierarchical msg with missing start'() {
        def messages = [
                leaveMsg(hierarchyMsgType1),
                flatMsg(flatMsgType2)
        ]
        def rootNode = FlightLogTreeBuilder.buildTree(messages)

        assert rootNode.children.size() == 2
        assert rootNode.enterMsg == null
        assert rootNode.leaveMsg == null

        assert rootNode.children[0].enterMsg == null
        assert rootNode.children[0].leaveMsg == messages[0]
        assert rootNode.children[0].children.empty

        assert rootNode.children[1].msg == messages[1]
    }

    @Test
    void 'decode hierarchical msg with flat payload and missing end'() {
        def messages = [
                flatMsg(flatMsgType1),
                enterMsg(hierarchyMsgType1),
                flatMsg(flatMsgType2)
        ]
        def rootNode = FlightLogTreeBuilder.buildTree(messages)

        assert rootNode.children.size() == 2
        assert rootNode.enterMsg == null
        assert rootNode.leaveMsg == null

        assert rootNode.children[0].msg == messages[0]

        assert rootNode.children[1].enterMsg == messages[1]
        assert rootNode.children[1].leaveMsg == null
        assert rootNode.children[1].children.size() == 1
        assert rootNode.children[1].children[0].msg == messages[2]
    }
}
