# Rehlds
Reverse-engineered (and bugfixed) HLDS

# What is this?
Rehlds is a result of reverse engineering of original HLDS (build 6152/6153) using DWARF debug info embedded into linux version of HLDS, engine_i486.so

Along with reverse engineering, a lot of defects and (potential) bugs were found and fixed

At this moment rehlds is not mature enough to use in production environment, however, during internal 3-day test on Windows on customized server (metamod + amxmodx + plugins) no issues were found

# Goals of the project
<ul>
<li>Provide more stable (than official) version of Half-Life dedicated server with extended API for mods and plugins</li>
<li>Performance optimizations (use of SSE for vector math for example) is another goal for the future</li>
</ul>

# How can use it?
Rehlds is fully compatible with official HLDS. All you have to do is to download rehlds binaries and replace original swds.dll/engine_i486.so

Compiled binaries are available here: http://nexus.rehlds.org/nexus/content/repositories/rehlds-releases/rehlds/rehlds/0.1/rehlds-0.1.zip

Archive's bin directory contains 2 subdirectories, 'bugfixed' and 'pure'
<ul>
<li>'pure' version is designed to work exactly as official hlds engine</li>
<li>'bugfixed' version contains some fixes and improvements</li>
</ul>

<b>Warning!</b> Rehlds is not binary compatible with original hlds since it's compiled with compilers other than ones used for original hlds. This means that plugins that do binary code analysis (Orpheu for example) probably will not work with rehlds.

# How can I help the project?
Just install it on your game server and report problems you faced
