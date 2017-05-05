## HLDS Launcher <img align="right" src="https://cloud.githubusercontent.com/assets/5860435/25316344/add057d4-288f-11e7-93ab-84706a388c3c.png" alt="HLDS Launcher"/>

## What is this?
Dedicated Server Launcher for Goldsrc based games

### Building
On Windows:
<pre>gradlew --max-workers=1 clean rehlds/dedicated:build</pre>

On Linux (ICC):
<pre>./gradlew --max-workers=1 clean rehlds/dedicated:build</pre>

On Linux (GCC):
<pre>./gradlew --max-workers=1 -PuseGcc clean rehlds/dedicated:build</pre>

Compiled binaries will be placed in the rehlds/dedicated/build/binaries/ directory
