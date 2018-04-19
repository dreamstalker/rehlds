/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

// Customization.h

#pragma once

#include "custom.h"

void		COM_ClearCustomizationList( struct customization_s *pHead, qboolean bCleanDecals);
qboolean	COM_CreateCustomization( struct customization_s *pListHead, struct resource_s *pResource, int playernumber, int flags, struct customization_s **pCustomization, int *nLumps ); 
int			COM_SizeofResourceList ( struct resource_s *pList, struct resourceinfo_s *ri );
