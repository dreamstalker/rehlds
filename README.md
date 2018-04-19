# Rehlds [![Build Status](http://teamcity.rehlds.org/app/rest/builds/buildType:(id:Rehlds_Publish)/statusIcon)](http://teamcity.rehlds.org/viewType.html?buildTypeId=Rehlds_Publish&guest=1) [![Download](https://camo.githubusercontent.com/65c70643ec7b40eea50971003624c2fb04d8d375/687474703a2f2f7265686c64732e6f72672f76657273696f6e2f7265686c64732e737667)](http://teamcity.rehlds.org/guestAuth/downloadArtifacts.html?buildTypeId=Rehlds_Publish&buildId=lastSuccessful)

Reverse-engineered (and bugfixed) HLDS

## What is this?
ReHLDS is a result of reverse engineering of original HLDS (build 6152/6153) using DWARF debug info embedded into linux version of HLDS, engine_i486.so

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

Compiled binaries are available here: http://nexus.rehlds.org/nexus/content/repositories/rehlds-dev/rehlds/rehlds/

Rehlds binaries require SSE, SSE2 and SSE3 instruction sets to run and can benefit from SSE4.1 and SSE4.2.

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
<li>syserror_logfile <filename> // File for the system error log. Default: sys_error.log
<li>sv_auto_precache_sounds_in_models <1|0> // Automatically precache sounds attached to models. Deault: 0
<li>sv_delayed_spray_upload <1|0> // Upload custom sprays after entering the game instead of when connecting. It increases upload speed. Default: 0
<li>sv_echo_unknown_cmd <1|0> // Echo in the console when trying execute an unknown command. Default: 0
<li>sv_rcon_condebug <1|0> // Print rcon debug in the console. Default: 1
<li>sv_force_ent_intersection <1|0> // In a 3-rd party plugins used to force colliding of SOLID_SLIDEBOX entities. Default: 0
<li>sv_rehlds_force_dlmax <1|0> // Force a client's cl_dlmax cvar to 1024. It avoids an excessive packets fragmentation. Default: 0
<li>sv_rehlds_hull_centering <1|0> // Use center of hull instead of corner. Default: 0
<li>sv_rehlds_movecmdrate_max_avg // Max average level of 'move' cmds for ban. Default: 400
<li>sv_rehlds_movecmdrate_avg_punish // Time in minutes for which the player will be banned (0 - Permanent, use a negative number for a kick). Default: 5
<li>sv_rehlds_movecmdrate_max_burst // Max burst level of 'move' cmds for ban. Default: 2500
<li>sv_rehlds_movecmdrate_burst_punish // Time in minutes for which the player will be banned (0 - Permanent, use a negative number for a kick). Default: 5
<li>sv_rehlds_send_mapcycle <1|0> // Send mapcycle.txt in serverinfo message (HLDS behavior, but it is unused on the client). Default: 0
<li>sv_rehlds_stringcmdrate_max_avg // Max average level of 'string' cmds for ban. Default: 80
<li>sv_rehlds_stringcmdrate_avg_punish // Time in minutes for which the player will be banned (0 - Permanent, use a negative number for a kick). Default: 5
<li>sv_rehlds_stringcmdrate_max_burst // Max burst level of 'string' cmds for ban. Default: 400
<li>sv_rehlds_stringcmdrate_burst_punish // Time in minutes for which the player will be banned (0 - Permanent, use a negative number for a kick). Default: 5
<li>sv_rehlds_userinfo_transmitted_fields // Userinfo fields only with these keys will be transmitted to clients via network. If not set then all fields will be transmitted (except prefixed with underscore). Each key must be prefixed by backslash, for example "\name\model\*sid\*hltv\bottomcolor\topcolor". Default: ""
<li>sv_rehlds_attachedentities_playeranimationspeed_fix // Fixes bug with gait animation speed increase when player has some attached entities (aiments). Can cause animation lags when cl_updaterate is low. Default: 0
</ul>

## Commands
Bugfixed version of rehlds contains an additional commands:
<ul>
<li>rescount // Prints the total count of precached resources in the server console
<li>reslist &lt;sound | model | decal | generic | event&gt; // Separately prints the details of the precached resources for sounds, models, decals, generic and events in server console. Useful for managing resources and dealing with the goldsource precache limits.
</ul>

## Build instructions
There are several software requirements for building rehlds:
<ol>
<li>Java Development Kit (JDK) 7+ (http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)</li>
<li>For Windows: Visual Studio 2013 and later</li>
<li>For Linux: Intel C++ Compiler 13 and later or GCC 4.9.2 or later (some earlier versions might work too)</li>
</ol>

### Checking requirements
#### JDK version
Windows<pre>&gt; %JAVA_HOME%\bin\javac -version
javac 1.8.0_25
</pre>

Linux
<pre>$ javac -version
javac 1.7.0_65
</pre>

#### Visual Studio
Help -> About

#### ICC
<pre>$ icc --version
icc (ICC) 15.0.1 20141023
</pre>

#### GCC
<pre>$ gcc --version
gcc (Debian 4.9.2-10) 4.9.2
</pre>

### Building
On Windows:
<pre>gradlew --max-workers=1 clean buildRelease</pre>
* For faster building without unit tests use this:exclamation:
<pre>gradlew --max-workers=1 clean buildFixes</pre>
<b>NOTE:</b> You can also use `Visual Studio` to build, just select from the solution configurations list `Release Swds` or `Debug Swds`<br />

On Linux (ICC):
<pre>./gradlew --max-workers=1 clean buildRelease</pre>
* For faster building without unit tests use this:exclamation:
<pre>./gradlew --max-workers=1 clean buildFixes</pre>

On Linux (GCC):
<pre>./gradlew --max-workers=1 -PuseGcc clean buildRelease</pre>
* For faster building without unit tests use this:exclamation:
<pre>./gradlew --max-workers=1 -PuseGcc clean buildFixes</pre>

Also there is a task `buildEngine`, it builds only engine, without other parts of the project.<br />
Compiled binaries will be placed in the rehlds/build/binaries/ directory

## How can I help the project?
Just install it on your game server and report problems you faced.
Merge requests are also welcome :)
