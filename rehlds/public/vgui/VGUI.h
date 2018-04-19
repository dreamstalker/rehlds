#pragma once

#ifdef _WIN32
	#define VGUI2_LIB "vgui2.dll"
#else
	#define VGUI2_LIB "vgui2.so"
#endif // _WIN32

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

} // namespace vgui2
