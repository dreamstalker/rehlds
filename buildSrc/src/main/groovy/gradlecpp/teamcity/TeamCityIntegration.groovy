package gradlecpp.teamcity

import groovy.transform.CompileStatic


class TeamCityIntegration {

    static final String flowId = System.getenv('TEAMCITY_PROCESS_FLOW_ID')
    static final boolean underTeamcity = System.getenv('TEAMCITY_PROJECT_NAME')
    static boolean writeOutput = underTeamcity

    @CompileStatic
    private static String escape(String s) {
        StringBuilder sb = new StringBuilder((int)(s.length() * 1.2));
        for (char c in s.chars) {
            switch (c) {
                case '\n': sb.append('|n'); break;
                case '\r': sb.append('|r'); break;
                case '\'': sb.append('|\''); break;
                case '|':  sb.append('||'); break;
                case ']':  sb.append('|]'); break;
                default:   sb.append(c);
            }
        }

        return sb.toString()
    }

    @CompileStatic
    static void writeMessage(String name, Map params) {
        if (!writeOutput) return
        StringBuilder sb = new StringBuilder()
        sb.append('##teamcity[').append(name)
        params.each { e ->
            if (e.value != null) {
                sb.append(' ').append('' + e.key).append('=\'').append(escape('' + e.value)).append('\'')
            }
        }
        sb.append(']')

        println sb.toString()
    }

    static void suiteStarted(String suiteName) {
        writeMessage('testSuiteStarted', [name: suiteName, flowId: flowId ?: null])
    }

    static void suiteFinished(String suiteName) {
        writeMessage('testSuiteFinished', [name: suiteName, flowId: flowId ?: null])
    }

    static void testStarted(String testName) {
        writeMessage('testStarted', [name: testName, flowId: flowId ?: null])
    }

    static void testStdOut(String testName, String output) {
        writeMessage('testStdOut', [name: testName, out: output, flowId: flowId ?: null])
    }

    static void testFinished(String testName, long durationMs) {
        writeMessage('testFinished', [
                name: testName,
                flowId: flowId ?: null,
                duration: (durationMs >= 0) ? durationMs : null
        ])
    }

    static void testFailed(String testName, String message, String details) {
        writeMessage('testFailed', [
                name: testName,
                flowId: flowId ?: null,
                message: message,
                details: details
        ])
    }

    static void testIgnored(String testName, String message) {
        writeMessage('testIgnored', [
                name: testName,
                flowId: flowId ?: null,
                message: message,
        ])
    }
}
