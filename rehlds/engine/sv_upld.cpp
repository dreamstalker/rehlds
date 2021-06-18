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

#include "precompiled.h"

// Checks MD5 of the resource against local cache and returns TRUE if resource was found or if downloads are disabled. Otherwise, if resource was requested from the player, it returns FALSE.
qboolean SV_CheckFile(sizebuf_t *msg, char *filename)
{
	resource_t p = { };

#ifdef REHLDS_FIXES

	// FIXED: First, check for allowed downloads, then try to lookup, this is faster if downloads aren't enabled
	if (sv_allow_upload.value == 0.0f)
	{
		// Downloads are not allowed, continue with the state we have
		return TRUE;
	}

	if (Q_strlen(filename) == 36 && Q_strnicmp(filename, "!MD5", 4) == 0)
	{
		// MD5 signature is correct, lets try to find this resource locally
		COM_HexConvert(filename + 4, 32, p.rgucMD5_hash);
		if (HPAK_GetDataPointer("custom.hpk", &p, 0, 0))
		{
			return TRUE;
		}
	}

#else // REHLDS_FIXES

	if (Q_strlen(filename) == 36 && Q_strnicmp(filename, "!MD5", 4) == 0)
	{
		// MD5 signature is correct, lets try to find this resource locally
		COM_HexConvert(filename + 4, 32, p.rgucMD5_hash);
		if (HPAK_GetDataPointer("custom.hpk", &p, 0, 0))
		{
			return TRUE;
		}
	}

	if (sv_allow_upload.value == 0.0f)
	{
		// Downloads are not allowed, continue with the state we have
		return TRUE;
	}

#endif // REHLDS_FIXES

#ifdef REHLDS_FIXES
	// While client is connecting he always send too small fragments (128 bytes)
	// But if client is fully connected he send fragments with cl_dlmax size
	// So, send upload in SV_SendEnts_f
	if (!sv_delayed_spray_upload.value)
#endif // REHLDS_FIXES
	{
		MSG_WriteByte(msg, svc_stufftext);
		MSG_WriteString(msg, va("upload \"!MD5%s\"\n", MD5_Print(p.rgucMD5_hash)));
	}

	return FALSE;
}

void SV_ClearResourceLists(client_t *cl)
{
	if (!cl)
	{
		Sys_Error("%s: SV_ClearResourceLists with NULL client!", __func__);
	}

	SV_ClearResourceList(&cl->resourcesneeded);
	SV_ClearResourceList(&cl->resourcesonhand);
}

// Reinitializes customizations list. Tries to create customization for each resource in on-hands list.
void SV_CreateCustomizationList(client_t *pHost)
{
	resource_t *pResource;

	// Clear customizations list
#ifdef REHLDS_FIXES
	// FIXED: Do it right, free mem, even if this is not actually needed (should be freed already on disconnect)
	COM_ClearCustomizationList(&pHost->customdata, FALSE);
#else // REHLDS_FIXES
	pHost->customdata.pNext = NULL;
#endif // REHLDS_FIXES

	for( pResource = pHost->resourcesonhand.pNext; pResource != &pHost->resourcesonhand; pResource = pResource->pNext )
	{
		// TODO: Check if we need to filter resources here based on type (t_decal) and flags (RES_CUSTOM)

		// Search if this resource already in customizations list
		qboolean bFound = FALSE;
		customization_t *pList = pHost->customdata.pNext;
		while (pList && Q_memcmp(pList->resource.rgucMD5_hash, pResource->rgucMD5_hash, 16))
		{
			pList = pList->pNext;
		}
		if (pList != NULL)
		{
			Con_DPrintf("SV_CreateCustomization list, ignoring dup. resource for player %s\n", pHost->name);
			bFound = TRUE;
		}

		if (!bFound)
		{
			// Try to create customization and add it to the list
			customization_t *pCust;
			qboolean bNoError;
			int nLumps = 0;
			bNoError = COM_CreateCustomization(&pHost->customdata, pResource, -1, FCUST_WIPEDATA | FCUST_FROMHPAK, &pCust, &nLumps);
			if (bNoError)
			{
				pCust->nUserData2 = nLumps;
				gEntityInterface.pfnPlayerCustomization(pHost->edict, pCust);
			}
			else
			{
				if (sv_allow_upload.value == 0.0f)
					Con_Printf("Ignoring custom decal from %s\n", pHost->name);
				else
					Con_Printf("Ignoring invalid custom decal from %s\n", pHost->name);
			}
		}
	}
}

// Sends resource to all other players, optionally skipping originating player.
void SV_Customization(client_t *pPlayer, resource_t *pResource, qboolean bSkipPlayer)
{
	int i;
	int nPlayerNumber;
	client_t *pHost;

	// Get originating player id
	for (i = 0, pHost = g_psvs.clients; i < g_psvs.maxclients; i++, pHost++)
	{
		if (pHost == pPlayer)
			break;
	}
	if (i == g_psvs.maxclients)
	{
		Sys_Error("%s: Couldn't find player index for customization.", __func__);
	}

	nPlayerNumber = i;

	// Send resource to all other active players
	for (i = 0, pHost = g_psvs.clients; i < g_psvs.maxclients; i++, pHost++)
	{
		if (pHost->fakeclient)
			continue;

		if (!pHost->active && !pHost->spawned)
			continue;

		if (pHost == pPlayer && bSkipPlayer)
			continue;

		MSG_WriteByte(&pHost->netchan.message, svc_customization);
		MSG_WriteByte(&pHost->netchan.message, nPlayerNumber);
		MSG_WriteByte(&pHost->netchan.message, pResource->type);
		MSG_WriteString(&pHost->netchan.message, pResource->szFileName);
		MSG_WriteShort(&pHost->netchan.message, pResource->nIndex);
		MSG_WriteLong(&pHost->netchan.message, pResource->nDownloadSize);
		MSG_WriteByte(&pHost->netchan.message, pResource->ucFlags);
		if (pResource->ucFlags & RES_CUSTOM)
		{
			SZ_Write(&pHost->netchan.message, pResource->rgucMD5_hash, 16);
		}
	}
}

// Creates customizations list for the current player and sends resources to other players.
void SV_RegisterResources(void)
{
	resource_t *pResource;
	client_t *pHost = host_client;

	pHost->uploading = FALSE;
#ifdef REHLDS_FIXES
	SV_CreateCustomizationList(pHost);		// FIXED: Call this function only once. It was crazy to call it for each resource available.
	for (pResource = pHost->resourcesonhand.pNext; pResource != &pHost->resourcesonhand; pResource = pResource->pNext)
	{
		SV_Customization(pHost, pResource, TRUE);
	}
#else // REHLDS_FIXES
	for (pResource = pHost->resourcesonhand.pNext; pResource != &pHost->resourcesonhand; pResource = pResource->pNext)
	{
		SV_CreateCustomizationList(pHost);
		SV_Customization(pHost, pResource, TRUE);
	}
#endif // REHLDS_FIXES
}

void SV_MoveToOnHandList(resource_t *pResource)
{
	if (!pResource)
	{
		Con_DPrintf("Null resource passed to SV_MoveToOnHandList\n");
		return;
	}

	SV_RemoveFromResourceList(pResource);
	SV_AddToResourceList(pResource, &host_client->resourcesonhand);
}

void SV_AddToResourceList(resource_t *pResource, resource_t *pList)
{
	if (pResource->pPrev || pResource->pNext)
	{
		Con_Printf("Resource already linked\n");
		return;
	}

	pResource->pPrev = pList->pPrev;
	pResource->pNext = pList;
	pList->pPrev->pNext = pResource;
	pList->pPrev = pResource;
}

void SV_ClearResourceList(resource_t *pList)
{
	resource_t *p, *n;

	for( p = pList->pNext; p && p != pList; p = n )
	{
		n = p->pNext;

		SV_RemoveFromResourceList(p);
		Mem_Free(p);
	}


	pList->pPrev = pList;
	pList->pNext = pList;
}

void SV_RemoveFromResourceList(resource_t *pResource)
{
	pResource->pPrev->pNext = pResource->pNext;
	pResource->pNext->pPrev = pResource->pPrev;
	pResource->pPrev = NULL;
	pResource->pNext = NULL;
}

// For each t_decal and RES_CUSTOM resource the player had shown to us, tries to find it locally or count size required to be downloaded.
int SV_EstimateNeededResources(void)
{
	resource_t *p;
	int missing;
	int size = 0;

	for (p = host_client->resourcesneeded.pNext; p != &host_client->resourcesneeded; p = p->pNext)
	{
#ifdef REHLDS_FIXES
		if (p->type == t_decal && (p->ucFlags & RES_CUSTOM) && p->nDownloadSize > 0)	// FIXED: We don't need to lookup if we can't or will not download anyway
#else // REHLDS_FIXES
		if (p->type == t_decal)
#endif // REHLDS_FIXES
		{
			missing = !HPAK_ResourceForHash("custom.hpk", p->rgucMD5_hash, NULL);
			if (missing)
			{
#ifdef REHLDS_FIXES
				// FIXED: Moved into outer scope
#else // REHLDS_FIXES
				if (p->nDownloadSize)
#endif // REHLDS_FIXES
				{
					size += p->nDownloadSize;
					p->ucFlags |= RES_WASMISSING;
				}
			}
		}
	}

	return size;
}

// This is called each frame to do checks on players if they uploaded all files that where requested from them.
void SV_RequestMissingResourcesFromClients(void)
{
	host_client = g_psvs.clients;
	for (int i = 0; i < g_psvs.maxclients; i++, host_client++)
	{
		if (!host_client->active && !host_client->spawned)
		{
			continue;
		}
		while (SV_RequestMissingResources())
			;	// Woot??!
	}
}

// Creates customizations list for a player (the current player actually, see the caller) and sends them out to other players when all needed resources are on-hands. Also sends other players customizations to the current player.
qboolean SV_UploadComplete(client_t *cl)
{
	if (cl->resourcesneeded.pNext == &cl->resourcesneeded)
	{
		// All resources are available locally, now we can do customizations propagation
		SV_RegisterResources();
		SV_PropagateCustomizations();
		if (sv_allow_upload.value != 0.0f)
		{
			Con_DPrintf("Custom resource propagation complete.\n");
		}
		cl->uploaddoneregistering = TRUE;
		return TRUE;
	}
	return FALSE;
}

// For each resource the player had shown to us, moves it to on-hands list. For t_decal and RES_CUSTOM it tries to find it locally or request resource from the player.
void SV_BatchUploadRequest(client_t *cl)
{
	resource_t *p, *n;
	char filename[MAX_PATH];

	for( p = cl->resourcesneeded.pNext; p != &cl->resourcesneeded; p = n )
	{
		n = p->pNext;

		if ((p->ucFlags & RES_WASMISSING) == 0)
		{
			SV_MoveToOnHandList(p);
		}
		else if (p->type == t_decal)
		{
			if (p->ucFlags & RES_CUSTOM)
			{
				Q_snprintf(filename, sizeof(filename), "!MD5%s", MD5_Print(p->rgucMD5_hash));
				if (SV_CheckFile(&cl->netchan.message, filename))
				{
					SV_MoveToOnHandList(p);
				}
			}
			else
			{
				Con_Printf("Non customization in upload queue!\n");
				SV_MoveToOnHandList(p);
			}
		}
	}
}

// This is used to do recurring checks on the current player that he uploaded all resources that where needed.
qboolean SV_RequestMissingResources(void)
{
	if (host_client->uploading && !host_client->uploaddoneregistering)
	{
		SV_UploadComplete(host_client);
	}
	return FALSE;
}

void SV_ParseResourceList(client_t *pSenderClient)
{
	int i, total;
	int totalsize;
	resource_t *resource;
	resourceinfo_t ri;

	total = MSG_ReadShort();

#ifdef REHLDS_FIXES
	SV_ClearResourceLists(host_client);
#else // REHLDS_FIXES
	SV_ClearResourceList(&host_client->resourcesneeded);
	SV_ClearResourceList(&host_client->resourcesonhand);
#endif // REHLDS_FIXES

#ifdef REHLDS_FIXES
	if (total > 1) // client uses only one custom resource (spray decal)
	{
		SV_DropClient(host_client, false, "Too many resources in client resource list");
		return;
	}
#endif // REHLDS_CHECKS

	for (i = 0; i < total; i++)
	{
		resource = (resource_t *)Mem_ZeroMalloc(sizeof(resource_t));
		Q_strncpy(resource->szFileName, MSG_ReadString(), sizeof(resource->szFileName) - 1);
		resource->szFileName[sizeof(resource->szFileName) - 1] = 0;
		resource->type = (resourcetype_t)MSG_ReadByte();
		resource->nIndex = MSG_ReadShort();
		resource->nDownloadSize = MSG_ReadLong();
		resource->ucFlags = MSG_ReadByte() & (~RES_WASMISSING);
		if (resource->ucFlags & RES_CUSTOM)
			MSG_ReadBuf(16, resource->rgucMD5_hash);
		resource->pNext = NULL;
		resource->pPrev = NULL;

#ifdef REHLDS_FIXES
		SV_AddToResourceList(resource, &host_client->resourcesneeded);	// FIXED: Mem leak. Add to list to free current resource in SV_ClearResourceList if something goes wrong.
#endif // REHLDS_FIXES

		if (msg_badread || resource->type > t_world ||
#ifdef REHLDS_FIXES
			resource->type != t_decal || !(resource->ucFlags & RES_CUSTOM) || Q_strcmp(resource->szFileName, "tempdecal.wad") != 0 || // client uses only tempdecal.wad for customization
			resource->nDownloadSize <= 0 ||		// FIXED: Check that download size is valid
#endif // REHLDS_FIXES
			resource->nDownloadSize > 1024 * 1024 * 1024)	// FIXME: Are they gone crazy??!
		{
#ifdef REHLDS_FIXES
			SV_ClearResourceLists(host_client);
#else // REHLDS_FIXES
			SV_ClearResourceList(&host_client->resourcesneeded);
			SV_ClearResourceList(&host_client->resourcesonhand);
#endif // REHLDS_FIXES
			return;
		}

#ifndef REHLDS_FIXES
		SV_AddToResourceList(resource, &host_client->resourcesneeded);
#endif // REHLDS_FIXES
	}

	if (sv_allow_upload.value != 0.0f)
	{
		Con_DPrintf("Verifying and uploading resources...\n");
		totalsize = COM_SizeofResourceList(&host_client->resourcesneeded, &ri);
#ifdef REHLDS_FIXES
		if (totalsize > 0)
#else // REHLDS_FIXES
		if (totalsize != 0)
#endif // REHLDS_FIXES
		{
			Con_DPrintf("Custom resources total %.2fK\n", total / 1024.0f);
#ifndef REHLDS_FIXES // because client can send only decals, why there is need to check other types?
			if (ri.info[t_model].size)
			{
				total = ri.info[t_model].size;
				Con_DPrintf("  Models:  %.2fK\n", total / 1024.0f);
			}
			if (ri.info[t_sound].size)
			{
				total = ri.info[t_sound].size;
				Con_DPrintf("  Sounds:  %.2fK\n", total / 1024.0f);
			}
			if (ri.info[t_decal].size)
			{
#endif // REHLDS_FIXES
			// this check is useless, because presence of decals was checked before.
				total = ri.info[t_decal].size;
				Con_DPrintf("  Decals:  %.2fK\n", total / 1024.0f);
#ifndef REHLDS_FIXES
			}
			if (ri.info[t_skin].size)
			{
				total = ri.info[t_skin].size;
				Con_DPrintf("  Skins :  %.2fK\n", total / 1024.0f);
			}
			if (ri.info[t_generic].size)
			{
				total = ri.info[t_generic].size;
				Con_DPrintf("  Generic :  %.2fK\n", total / 1024.0f);
			}
			if (ri.info[t_eventscript].size)
			{
				total = ri.info[t_eventscript].size;
				Con_DPrintf("  Events  :  %.2fK\n", total / 1024.0f);
			}
#endif // REHLDS_FIXES
			Con_DPrintf("----------------------\n");

			int bytestodownload = SV_EstimateNeededResources();
			if (bytestodownload > sv_max_upload.value * 1024 * 1024)
			{
#ifdef REHLDS_FIXES
				SV_ClearResourceLists(host_client);
#else // REHLDS_FIXES
				SV_ClearResourceList(&host_client->resourcesneeded);
				SV_ClearResourceList(&host_client->resourcesonhand);
#endif //REHLDS_FIXES
				return;
			}

			if (bytestodownload > 1024)
				Con_DPrintf("Resources to request: %.2fK\n", bytestodownload / 1024.0f);
			else
				Con_DPrintf("Resources to request: %i bytes\n", bytestodownload);
		}
	}

	host_client->uploading = TRUE;
	host_client->uploaddoneregistering = FALSE;

	SV_BatchUploadRequest(host_client);
}
