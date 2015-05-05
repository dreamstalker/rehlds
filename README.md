# Rehlds
Reverse-engineered (and bugfixed) HLDS

## What is this?
Rehlds is a result of reverse engineering of original HLDS (build 6152/6153) using DWARF debug info embedded into linux version of HLDS, engine_i486.so

Along with reverse engineering, a lot of defects and (potential) bugs were found and fixed

At this moment rehlds is not mature enough to use in production environment, however, during internal 3-day test on Windows on customized server (metamod + amxmodx + plugins) no issues were found

## Goals of the project
<ul>
<li>Provide more stable (than official) version of Half-Life dedicated server with extended API for mods and plugins</li>
<li>Performance optimizations (use of SSE for vector math for example) is another goal for the future</li>
</ul>

## How can use it?
Rehlds is fully compatible with official HLDS. All you have to do is to download rehlds binaries and replace original swds.dll/engine_i486.so

Compiled binaries are available here: http://nexus.rehlds.org/nexus/content/repositories/rehlds-releases/rehlds/rehlds/0.1/rehlds-0.1.zip

Archive's bin directory contains 2 subdirectories, 'bugfixed' and 'pure'
<ul>
<li>'pure' version is designed to work exactly as official hlds engine</li>
<li>'bugfixed' version contains some fixes and improvements</li>
</ul>

<b>Warning!</b> Rehlds is not binary compatible with original hlds since it's compiled with compilers other than ones used for original hlds. This means that plugins that do binary code analysis (Orpheu for example) probably will not work with rehlds.

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

Compiled binaries will be placed in the build/binaries/ directory

## How can I help the project?
Just install it on your game server and report problems you faced
Merge requests are also welcome :)
