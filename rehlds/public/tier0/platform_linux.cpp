//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "precompiled.h"

double Plat_FloatTime()
{
	struct timeval  tp;
	static int      secbase = 0;

	gettimeofday(&tp, NULL);

	if (!secbase)
	{
		secbase = tp.tv_sec;
		return (tp.tv_usec / 1000000.0);
	}

	return ((tp.tv_sec - secbase) + tp.tv_usec / 1000000.0);
}

unsigned long Plat_MSTime()
{
	struct timeval  tp;
	static int      secbase = 0;

	gettimeofday(&tp, NULL);

	if (!secbase)
	{
		secbase = tp.tv_sec;
		return (tp.tv_usec / 1000000.0);
	}

	return (unsigned long)((tp.tv_sec - secbase) + tp.tv_usec / 1000000.0);

}



bool vtune(bool resume)
{
	return true;
}


// -------------------------------------------------------------------------------------------------- //
// Memory stuff.
// -------------------------------------------------------------------------------------------------- //


void Plat_SetThreadName(unsigned long dwThreadID, const char *pName)
{
	Assert("Plat_SetThreadName not implemented");
}
