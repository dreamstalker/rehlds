#pragma once

#include "osconfig.h"
#include "archtypes.h"
#include "mathlib.h"
#include "FileSystem.h"

#include "common.h"
#include "mem.h"

#include "engine_hlds_api.h"
#include "idedicatedexports.h"
#include "icommandline.h"

#ifdef LAUNCHER_FIXES
struct DLL_FUNCTIONS;
#include "cvardef.h"
#include "rehlds_api.h"
#endif

#include "sys_ded.h"
#include "icommandline.h"
#include "textconsole.h"
#include "vgui/vguihelpers.h"

#include "IObjectContainer.h"
#include "ObjectList.h"

#ifdef _WIN32
	#include "conproc.h"
#endif // _WIN32
