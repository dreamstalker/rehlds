/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#ifndef PR_DLLS_H
#define PR_DLLS_H
#ifdef _WIN32
#pragma once
#endif

#include "maintypes.h"
#include "eiface.h"


/* <8a763> ../engine/pr_dlls.h:42 */
typedef struct functiontable_s
{
	uint32 pFunction;
	char *pFunctionName;
} functiontable_t;

/* 572 */
/* <8a793> ../engine/pr_dlls.h:48 */
typedef struct extensiondll_s
{
	void *lDLLHandle;
	functiontable_t *functionTable;
	int functionCount;
} extensiondll_t;

/* <8a7db> ../engine/pr_dlls.h:63 */
typedef void(*ENTITYINIT)(struct entvars_s *);
/* <8a7f8> ../engine/pr_dlls.h:64 */
typedef void(*DISPATCHFUNCTION)(struct entvars_s *, void *);
/* <8a81a> ../engine/pr_dlls.h:65 */
typedef void(*FIELDIOFUNCTION)(SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int);

#endif // PR_DLLS_H
