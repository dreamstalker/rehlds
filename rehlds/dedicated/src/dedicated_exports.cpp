#include "precompiled.h"

class CDedicatedExports: IDedicatedExports {
public:
	void Sys_Printf(char *text);
};

EXPOSE_SINGLE_INTERFACE(CDedicatedExports, IDedicatedExports, VENGINE_DEDICATEDEXPORTS_API_VERSION);

void CDedicatedExports::Sys_Printf(char *text)
{
	::Sys_Printf_Safe(text);
}
