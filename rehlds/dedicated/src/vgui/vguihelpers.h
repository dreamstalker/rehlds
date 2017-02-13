#pragma once

int StartVGUI();
void StopVGUI();
void RunVGUIFrame();
bool VGUIIsRunning();
bool VGUIIsStopping();
bool VGUIIsInConfig();
void VGUIFinishedConfig();
void VGUIPrintf(const char *msg);
