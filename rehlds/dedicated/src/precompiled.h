#pragma once

#include <atomic>

#include "basetypes.h"
#include "FileSystem.h"
#include "strtools.h"

#include "common.h"
#include "mem.h"

#include "engine_hlds_api.h"
#include "idedicatedexports.h"
#include "icommandline.h"
#include "isys.h"
#include "dll_state.h"
#include "dedicated.h"

#include "sys_ded.h"
#include "textconsole.h"
#include "vgui/vguihelpers.h"

#include "IObjectContainer.h"
#include "ObjectList.h"

#ifdef _WIN32
	#include "conproc.h"
	#include <mmsystem.h> // timeGetTime
#else
	#include <signal.h>
#endif // _WIN32
