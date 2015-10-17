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


/* <7f360> ../engine/pr_edict.c:37 */
void ED_ClearEdict(edict_t *e)
{
	Q_memset(&e->v, 0, sizeof(e->v));
	e->free = FALSE;
	ReleaseEntityDLLFields(e);
	InitEntityDLLFields(e);
}

/* <7f3cb> ../engine/pr_edict.c:57 */
edict_t *ED_Alloc(void)
{
	int i;
	edict_t *e;

	// Search for free entity
	for (i = g_psvs.maxclients + 1; i < g_psv.num_edicts; i++)
	{
		e = &g_psv.edicts[i];
		if (e->free && (e->freetime <= 2.0 || g_psv.time - e->freetime >= 0.5))
		{
			ED_ClearEdict(e);
			return e;
		}
	}

	// Check if we are out of free edicts
	if (i >= g_psv.max_edicts)
	{
		if (!g_psv.max_edicts)
		{
			Sys_Error(__FUNCTION__ ": no edicts yet");
		}
		Sys_Error(__FUNCTION__ ": no free edicts");
	}

	// Use new one
	++g_psv.num_edicts;
	e = &g_psv.edicts[i];

	ED_ClearEdict(e);
	return e;
}

/* <7f310> ../engine/pr_edict.c:96 */
void ED_Free(edict_t *ed)
{
	if (!ed->free)
	{
		SV_UnlinkEdict(ed);
		FreeEntPrivateData(ed);
		ed->serialnumber++;
		ed->freetime = (float)g_psv.time;
		ed->free = TRUE;
		ed->v.flags = 0;
		ed->v.model = 0;

		ed->v.takedamage = 0;
		ed->v.modelindex = 0;
		ed->v.colormap = 0;
		ed->v.skin = 0;
		ed->v.frame = 0;
		ed->v.scale = 0;
		ed->v.gravity = 0;
		ed->v.nextthink = -1.0;
		ed->v.solid = SOLID_NOT;

		ed->v.origin[0] = vec3_origin[0];
		ed->v.origin[1] = vec3_origin[1];
		ed->v.origin[2] = vec3_origin[2];
		ed->v.angles[0] = vec3_origin[0];
		ed->v.angles[1] = vec3_origin[1];
		ed->v.angles[2] = vec3_origin[2];
	}
}

/* <7f4f6> ../engine/pr_edict.c:170 */
NOXREF void ED_Count(void)
{
	NOXREFCHECK;

	int i;
	edict_t *ent;
	int active = 0, models = 0, solid = 0, step = 0;

	for (i = 0; i < g_psv.num_edicts; i++)
	{
		ent = &g_psv.edicts[i];
		if (!ent->free)
		{
			++active;
			models += (ent->v.model) ? 1 : 0;
			solid += (ent->v.solid) ? 1 : 0;
			step += (ent->v.movetype == MOVETYPE_STEP) ? 1 : 0;
		}
	}

	Con_Printf("num_edicts:%3i\n", g_psv.num_edicts);
	Con_Printf("active    :%3i\n", active);
	Con_Printf("view      :%3i\n", models);
	Con_Printf("touch     :%3i\n", solid);
	Con_Printf("step      :%3i\n", step);
}

/* <7f568> ../engine/pr_edict.c:205 */
char *ED_NewString(const char *string)
{
	char *new_s;
	
	//Engine string pooling
#ifdef REHLDS_FIXES

	//escaping is done inside Ed_StrPool_Alloc()
	new_s = Ed_StrPool_Alloc(string);
	
#else //REHLDS_FIXES

	int l = Q_strlen(string);
	new_s = (char *)Hunk_Alloc(l + 1);
	char* new_p = new_s;

	for (int i = 0; i < l; i++, new_p++)
	{
		if (string[i] == '\\')
		{
			if (string[i + 1] == 'n')
				*new_p = '\n';
			else
				*new_p = '\\';
			i++;
		}
		else
		{
			*new_p = string[i];
		}
	}
	*new_p = 0;

#endif //REHLDS_FIXES

	return new_s;
}

/* <7f5ee> ../engine/pr_edict.c:245 */
char *ED_ParseEdict(char *data, edict_t *ent)
{
	qboolean init = FALSE;
	char keyname[256];
	int n;
	ENTITYINIT pEntityInit;
	char *className;
	KeyValueData kvd;

	if (ent != g_psv.edicts)
	{
		Q_memset(&ent->v, 0, sizeof(ent->v));
	}

	InitEntityDLLFields(ent);

	if (SuckOutClassname(data, ent))
	{
		className = (char *)(pr_strings + ent->v.classname);

		pEntityInit = GetEntityInit(className);
		if (pEntityInit)
		{
			pEntityInit(&ent->v);
			init = TRUE;
		}
		else
		{
			pEntityInit = GetEntityInit("custom");
			if (pEntityInit)
			{
				pEntityInit(&ent->v);
				kvd.szClassName = "custom";
				kvd.szKeyName = "customclass";
				kvd.szValue = className;
				kvd.fHandled = FALSE;
				gEntityInterface.pfnKeyValue(ent, &kvd);
				init = TRUE;
			}
			else
			{
				Con_DPrintf("Can't init %s\n", className);
				init = FALSE;
			}
		}

		while (1)
		{
			data = COM_Parse(data);
			if (com_token[0] == '}')
			{
				break;
			}
			if (!data)
			{
				Host_Error(__FUNCTION__ ": EOF without closing brace");
			}

			Q_strncpy(keyname, com_token, ARRAYSIZE(keyname) - 1);
			keyname[ARRAYSIZE(keyname) - 1] = 0;
			// Remove tail spaces
			for (n = Q_strlen(keyname) - 1; n >= 0 && keyname[n] == ' '; n--)
			{
				keyname[n] = 0;
			}

			data = COM_Parse(data);
			if (!data)
			{
				Host_Error(__FUNCTION__ ": EOF without closing brace");
			}
			if (com_token[0] == '}')
			{
				Host_Error(__FUNCTION__ ": closing brace without data");
			}

			if (className != NULL && !Q_strcmp(className, com_token))
			{
				continue;
			}

			if (!Q_strcmp(keyname, "angle"))
			{
				float value = (float)Q_atof(com_token);
				if (value >= 0.0)
				{
					Q_snprintf(com_token, COM_TOKEN_LEN, "%f %f %f", ent->v.angles[0], value, ent->v.angles[2]);
				}
				else if ((int)value == -1)
				{
					Q_snprintf(com_token, COM_TOKEN_LEN, "-90 0 0");
				}
				else
				{
					Q_snprintf(com_token, COM_TOKEN_LEN, "90 0 0");
				}

				Q_strcpy(keyname, "angles");
			}

			kvd.szClassName = className;
			kvd.szKeyName = keyname;
			kvd.szValue = com_token;
			kvd.fHandled = 0;
			gEntityInterface.pfnKeyValue(ent, &kvd);
		}
	}

	if (!init)
	{
		ent->free = 1;
		ent->serialnumber++;
	}
	return data;
}

/* <7f6ee> ../engine/pr_edict.c:402 */
void ED_LoadFromFile(char *data)
{
	edict_t *ent;
	int inhibit;

	gGlobalVariables.time = (float)g_psv.time;

	ent = NULL;
	inhibit = 0;
	while (1)
	{
		data = COM_Parse(data);
		if (!data)
		{
			break;
		}
		if (com_token[0] != '{')
		{
			Host_Error(__FUNCTION__ ": found %s when expecting {", com_token);
		}

		if (ent)
		{
			ent = ED_Alloc();
		}
		else
		{
			ent = g_psv.edicts;
			ReleaseEntityDLLFields(g_psv.edicts);	// TODO: May be better to call ED_ClearEdict here?
			InitEntityDLLFields(ent);
		}

		data = ED_ParseEdict(data, ent);
		if (ent->free)
		{
			continue;
		}

		if (deathmatch.value != 0.0 && (ent->v.spawnflags & SF_NOTINDEATHMATCH))
		{
			ED_Free(ent);
			++inhibit;
		}
		else
		{
			if (ent->v.classname)
			{
				if (gEntityInterface.pfnSpawn(ent) < 0 || (ent->v.flags & FL_KILLME))
				{
					ED_Free(ent);
				}
			}
			else
			{
				Con_Printf("No classname for:\n");
				ED_Free(ent);
			}
		}
	}
	Con_DPrintf("%i entities inhibited\n", inhibit);
}

/* <7f7fa> ../engine/pr_edict.c:484 */
NOXREF void PR_Init(void)
{
	NOXREFCHECK;
}

/* <7f80f> ../engine/pr_edict.c:495 */
edict_t *EDICT_NUM(int n)
{
	if (n < 0 || n >= g_psv.max_edicts)
	{
		Sys_Error(__FUNCTION__ ": bad number %i", n);
	}
	return &g_psv.edicts[n];
}

/* <7f84b> ../engine/pr_edict.c:511 */
int NUM_FOR_EDICT(const edict_t *e)
{
	int b;
	b = e - g_psv.edicts;

	if (b < 0 || b >= g_psv.num_edicts)
	{
		Sys_Error(__FUNCTION__ ": bad pointer");
	}

	return b;
}

/* <7f280> ../engine/pr_edict.c:539 */
bool SuckOutClassname(char *szInputStream, edict_t *pEdict)
{
	char szKeyName[256];
	KeyValueData kvd;

	// key
	szInputStream = COM_Parse(szInputStream);
	while (szInputStream && com_token[0] != '}')
	{
		Q_strncpy(szKeyName, com_token, ARRAYSIZE(szKeyName) - 1);
		szKeyName[ARRAYSIZE(szKeyName) - 1] = 0;

		// value
		szInputStream = COM_Parse(szInputStream);

		if (!Q_strcmp(szKeyName, "classname"))
		{
			kvd.szClassName = NULL;
			kvd.szKeyName = szKeyName;
			kvd.szValue = com_token;
			kvd.fHandled = FALSE;

			gEntityInterface.pfnKeyValue(pEdict, &kvd);

			if (kvd.fHandled == FALSE)
			{
				Host_Error(__FUNCTION__ ": parse error");
			}

			return true;
		}

		if (!szInputStream)
		{
			break;
		}

		// next key
		szInputStream = COM_Parse(szInputStream);
	}

#ifdef REHLDS_FIXES
	if (pEdict == g_psv.edicts)
	{
		kvd.szClassName = NULL;
		kvd.szKeyName = "classname";
		kvd.szValue = "worldspawn";
		kvd.fHandled = FALSE;

		gEntityInterface.pfnKeyValue(pEdict, &kvd);

		if (kvd.fHandled == FALSE)
		{
			Host_Error(__FUNCTION__ ": parse error");
		}

		return true;
	}
#endif

	// classname not found
	return false;
}

/* <7f89a> ../engine/pr_edict.c:579 */
void ReleaseEntityDLLFields(edict_t *pEdict)
{
	FreeEntPrivateData(pEdict);
}

/* <7f264> ../engine/pr_edict.c:584 */
void InitEntityDLLFields(edict_t *pEdict)
{
	pEdict->v.pContainingEntity = pEdict;
}

/* <7f8ee> ../engine/pr_edict.c:594 */
void* EXT_FUNC PvAllocEntPrivateData(edict_t *pEdict, int32 cb)
{
	FreeEntPrivateData(pEdict);

	if (cb <= 0)
	{
		return NULL;
	}

	pEdict->pvPrivateData = Mem_Calloc(1, cb);

#ifdef REHLDS_FLIGHT_REC
	if (rehlds_flrec_pvdata.string[0] != '0') {
		FR_AllocEntPrivateData(pEdict->pvPrivateData, cb);
	}
#endif //REHLDS_FLIGHT_REC

	return pEdict->pvPrivateData;
}

/* <7f943> ../engine/pr_edict.c:607 */
void* EXT_FUNC PvEntPrivateData(edict_t *pEdict)
{
	if (!pEdict)
	{
		return NULL;
	}

	return pEdict->pvPrivateData;
}

/* <7f2f4> ../engine/pr_edict.c:618 */
void FreeEntPrivateData(edict_t *pEdict)
{
	if (pEdict->pvPrivateData)
	{
		if (gNewDLLFunctions.pfnOnFreeEntPrivateData)
		{
			gNewDLLFunctions.pfnOnFreeEntPrivateData(pEdict);
		}

#ifdef REHLDS_FLIGHT_REC
		if (rehlds_flrec_pvdata.string[0] != '0') {
			FR_FreeEntPrivateData(pEdict->pvPrivateData);
		}
#endif //REHLDS_FLIGHT_REC

		Mem_Free(pEdict->pvPrivateData);
		pEdict->pvPrivateData = 0;
	}
}

/* <7f98e> ../engine/pr_edict.c:647 */
void FreeAllEntPrivateData(void)
{
	for (int i = 0; i < g_psv.num_edicts; i++)
	{
		FreeEntPrivateData(&g_psv.edicts[i]);
	}
}

/* <7f9d0> ../engine/pr_edict.c:660 */
edict_t* EXT_FUNC PEntityOfEntOffset(int iEntOffset)
{
	return (edict_t *)((char *)g_psv.edicts + iEntOffset);
}

/* <7f9fd> ../engine/pr_edict.c:665 */
int EXT_FUNC EntOffsetOfPEntity(const edict_t *pEdict)
{
	return (char *)pEdict - (char *)g_psv.edicts;
}

/* <7fa2a> ../engine/pr_edict.c:670 */
int EXT_FUNC IndexOfEdict(const edict_t *pEdict)
{
	int index = 0;
	if (pEdict)
	{
		index = pEdict - g_psv.edicts;
#ifdef REHLDS_FIXES
		if (index < 0 || index >= g_psv.max_edicts) //max_edicts is not valid entity index
#else // REHLDS_FIXES
		if (index < 0 || index > g_psv.max_edicts)
#endif // REHLDS_FIXES
		{
			Sys_Error(__FUNCTION__ ": bad entity");
		}
	}
	return index;
}

/* <7fa68> ../engine/pr_edict.c:691 */
edict_t* EXT_FUNC PEntityOfEntIndex(int iEntIndex)
{
	if (iEntIndex < 0 || iEntIndex >= g_psv.max_edicts)
	{
		return NULL;
	}

	edict_t *pEdict = EDICT_NUM(iEntIndex);

#ifdef REHLDS_FIXES
	if (pEdict && (pEdict->free || (iEntIndex > g_psvs.maxclients && !pEdict->pvPrivateData))) //Simplified confition
	{
		return NULL;
	}
#else // REHLDS_FIXES
	if ((!pEdict || pEdict->free || !pEdict->pvPrivateData) && (iEntIndex >= g_psvs.maxclients || pEdict->free))
	{
		if (iEntIndex >= g_psvs.maxclients || pEdict->free)
		{
			pEdict = NULL;
		}
	}
#endif // REHLDS_FIXES

	return pEdict;
}

/* <7fac3> ../engine/pr_edict.c:711 */
const char* EXT_FUNC SzFromIndex(int iString)
{
	return (const char *)(pr_strings + iString);
}

/* <7faf0> ../engine/pr_edict.c:721 */
entvars_t* EXT_FUNC GetVarsOfEnt(edict_t *pEdict)
{
	return &pEdict->v;
}

/* <7fb23> ../engine/pr_edict.c:731 */
edict_t* EXT_FUNC FindEntityByVars(entvars_t *pvars)
{
	for (int i = 0; i < g_psv.num_edicts; i++)
	{
		edict_t *pEdict = &g_psv.edicts[i];
		if (&pEdict->v == pvars)
		{
			return pEdict;
		}
	}
	return NULL;
}

/* <7fb75> ../engine/pr_edict.c:747 */
float EXT_FUNC CVarGetFloat(const char *szVarName)
{
	return Cvar_VariableValue(szVarName);
}

/* <7fba2> ../engine/pr_edict.c:753 */
const char* EXT_FUNC CVarGetString(const char *szVarName)
{
	return Cvar_VariableString(szVarName);
}

/* <7fbcf> ../engine/pr_edict.c:759 */
cvar_t* EXT_FUNC CVarGetPointer(const char *szVarName)
{
	return Cvar_FindVar(szVarName);
}

/* <7fbfc> ../engine/pr_edict.c:765 */
void EXT_FUNC CVarSetFloat(const char *szVarName, float flValue)
{
	Cvar_SetValue(szVarName, flValue);
}

/* <7fc34> ../engine/pr_edict.c:771 */
void EXT_FUNC CVarSetString(const char *szVarName, const char *szValue)
{
	Cvar_Set(szVarName, szValue);
}

/* <7fc6c> ../engine/pr_edict.c:777 */
void EXT_FUNC CVarRegister(cvar_t *pCvar)
{
	if (pCvar)
	{
		pCvar->flags |= FCVAR_EXTDLL;
		Cvar_RegisterVariable(pCvar);
	}
}

/* <7fc95> ../engine/pr_edict.c:794 */
int EXT_FUNC AllocEngineString(const char *szValue)
{
	return ED_NewString(szValue) - pr_strings;
}

/* <7fd06> ../engine/pr_edict.c:802 */
void EXT_FUNC SaveSpawnParms(edict_t *pEdict)
{
	int eoffset = NUM_FOR_EDICT(pEdict);
	if (eoffset < 1 || eoffset > g_psvs.maxclients)
	{
		Host_Error("Entity is not a client");
	}
	// Nothing more for this function even on client-side
}

/* <7fd78> ../engine/pr_edict.c:815 */
void* EXT_FUNC GetModelPtr(edict_t *pEdict)
{
	if (!pEdict)
	{
		return NULL;
	}
	return Mod_Extradata(g_psv.models[pEdict->v.modelindex]);
}
