# Rehlds
Reverse-engineered (and bugfixed) HLDS

## What is this?
Rehlds is a result of reverse engineering of original HLDS (build 6152/6153) using DWARF debug info embedded into linux version of HLDS, engine_i486.so

Along with reverse engineering, a lot of defects and (potential) bugs were found and fixed

You can try play on one of the servers that using rehlds: http://www.gametracker.com/search/?search_by=server_variable&search_by2=sv_version

## Goals of the project
<ul>
<li>Provide more stable (than official) version of Half-Life dedicated server with extended API for mods and plugins</li>
<li>Performance optimizations (use of SSE for vector math for example) is another goal for the future</li>
</ul>

## How can use it?
Rehlds is fully compatible with latest official HLDS downloaded by steamcmd. All you have to do is to download rehlds binaries and replace original swds.dll/engine_i486.so. For windows you can also copy a swds.pdb file with a debug information.
<br /><b>Warning!</b> Rehlds is not compatible with an old 5xxx or below platforms downloaded by hldsupdatetool.

Compiled binaries are available here: http://nexus.rehlds.org/nexus/content/repositories/rehlds-snapshots/rehlds/rehlds/0.2-SNAPSHOT/

Archive's bin directory contains 2 subdirectories, 'bugfixed' and 'pure'
<ul>
<li>'pure' version is designed to work exactly as official hlds engine</li>
<li>'bugfixed' version contains some fixes and improvements</li>
</ul>

<b>Warning!</b> Rehlds is not binary compatible with original hlds since it's compiled with compilers other than ones used for original hlds. This means that plugins that do binary code analysis (Orpheu for example) probably will not work with rehlds.

## Configuring
Bugfixed version of rehlds contains an additional cvars:
<ul>
<li>listipcfgfile <filename> // File for permanent ip bans. Default: listip.cfg
<li>sv_auto_precache_sounds_in_models <1|0> // Automatically precache sounds attached to models. Deault: 0
<li>sv_delayed_spray_upload <1|0> // Upload custom sprays after entering the game instead of when connecting. It increases upload speed. Default: 0
<li>sv_echo_unknown_cmd <1|0> // Echo in the console when trying execute an uncknown command. Default: 0
<li>sv_force_ent_intersection <1|0> // In a 3-rd party plugins used to force colliding of SOLID_SLIDEBOX entities. Default: 0
<li>sv_rehlds_force_dlmax <1|0> // Force a client's cl_dlmax cvar to 1024. It avoids an excessive packets fragmentation. Default: 0
<li>sv_rehlds_movecmdrate_max_avg // Max average level of 'move' cmds for ban. Default: 400
<li>sv_rehlds_movecmdrate_max_burst // Max burst level of 'move' cmds for ban. Default: 2500
<li>sv_rehlds_stringcmdrate_max_avg // Max average level of 'string' cmds for ban. Default: 80
<li>sv_rehlds_stringcmdrate_max_burst // Max burst level of 'string' cmds for ban. Default: 400
</ul>

## Build instructions
There are several software requirements for building rehlds:
<ol>
<li>Java Development Kit (JDK) 7+ (http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)</li>
<li>For Windows: Visual Studio 2013 and later</li>
<li>For Linux: Intel C++ Compiler 13 and later</li>
</ol>

### Checking requirements
####JDK version
Windows<pre>&gt; %JAVA_HOME%\bin\javac -version
javac 1.8.0_25
</pre>

Linux
<pre>$ javac -version
javac 1.7.0_65
</pre>

####Visual Studio
Help -> About

####ICC
<pre>$ icc --version
icc (ICC) 15.0.1 20141023
</pre>

### Building
On Windows:
<pre>gradlew --max-workers=1 clean buildRelease</pre>

On Linux:
<pre>./gradlew --max-workers=1 clean buildRelease</pre>

Compiled binaries will be placed in the rehlds/build/binaries/ directory

## How can I help the project?
Just install it on your game server and report problems you faced
Merge requests are also welcome :)
