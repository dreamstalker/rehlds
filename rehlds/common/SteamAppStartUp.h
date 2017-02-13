#ifndef STEAMAPPSTARTUP_H
#define STEAMAPPSTARTUP_H
#ifdef _WIN32
#pragma once
#endif

// Call this first thing at startup
// Works out if the app is a steam app that is being ran outside of steam,
// and if so, launches steam and tells it to run us as a steam app
//
// if it returns true, then exit
// if it ruturns false, then continue with normal startup
bool ShouldLaunchAppViaSteam(const char *cmdLine, const char *steamFilesystemDllName, const char *stdioFilesystemDllName);

#endif // STEAMAPPSTARTUP_H
