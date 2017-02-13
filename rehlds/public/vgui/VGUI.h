#ifndef VGUI_H
#define VGUI_H
#ifdef _WIN32
#pragma once
#endif

namespace vgui2
{

// handle to an internal vgui panel
// this is the only handle to a panel that is valid across dll boundaries
typedef unsigned int VPANEL;

// handles to vgui objects
// NULL values signify an invalid value
typedef unsigned long HScheme;
typedef unsigned long HTexture;
typedef unsigned long HCursor;

typedef int HContext;
typedef unsigned long HPanel;
typedef unsigned long HFont;

// the value of an invalid font handle
const VPANEL NULL_PANEL = 0;
const HFont INVALID_FONT = 0;
const HPanel INVALID_PANEL = 0xffffffff;

} // namespace vgui

#endif // VGUI_H
