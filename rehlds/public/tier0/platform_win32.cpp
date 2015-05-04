//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "precompiled.h"

static LARGE_INTEGER g_PerformanceFrequency;
static LARGE_INTEGER g_MSPerformanceFrequency;
static LARGE_INTEGER g_ClockStart;

static void InitTime()
{
	if (!g_PerformanceFrequency.QuadPart)
	{
		QueryPerformanceFrequency(&g_PerformanceFrequency);
		g_MSPerformanceFrequency.QuadPart = g_PerformanceFrequency.QuadPart / 1000;
		QueryPerformanceCounter(&g_ClockStart);
	}
}

double Plat_FloatTime()
{
	InitTime();

	LARGE_INTEGER CurrentTime;

	QueryPerformanceCounter(&CurrentTime);

	double fRawSeconds = (double)(CurrentTime.QuadPart - g_ClockStart.QuadPart) / (double)(g_PerformanceFrequency.QuadPart);

	return fRawSeconds;
}

unsigned long Plat_MSTime()
{
	InitTime();

	LARGE_INTEGER CurrentTime;

	QueryPerformanceCounter(&CurrentTime);

	return (unsigned long)((CurrentTime.QuadPart - g_ClockStart.QuadPart) / g_MSPerformanceFrequency.QuadPart);
}

bool vtune(bool resume)
{
	static bool bInitialized = false;
	static void(__cdecl *VTResume)(void) = NULL;
	static void(__cdecl *VTPause) (void) = NULL;

	// Grab the Pause and Resume function pointers from the VTune DLL the first time through:
	if (!bInitialized)
	{
		bInitialized = true;

		HINSTANCE pVTuneDLL = LoadLibrary("vtuneapi.dll");

		if (pVTuneDLL)
		{
			VTResume = (void(__cdecl *)())GetProcAddress(pVTuneDLL, "VTResume");
			VTPause = (void(__cdecl *)())GetProcAddress(pVTuneDLL, "VTPause");
		}
	}

	// Call the appropriate function, as indicated by the argument:
	if (resume && VTResume)
	{
		VTResume();
		return true;

	}
	else if (!resume && VTPause)
	{
		VTPause();
		return true;
	}

	return false;

}


// -------------------------------------------------------------------------------------------------- //
// Memory stuff.
// -------------------------------------------------------------------------------------------------- //
