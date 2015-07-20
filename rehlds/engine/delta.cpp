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


#ifndef Defines_and_Variables_region

/* <23bb1> ../engine/delta.c:47 */
typedef struct delta_link_s
{
	delta_link_t *next;
	delta_description_t *delta;
} delta_link_t;

/* <23be5> ../engine/delta.c:53 */
typedef struct delta_definition_s
{
	char *fieldName;
	int fieldOffset;
} delta_definition_t;

/* <23c19> ../engine/delta.c:59 */
typedef struct delta_definition_list_s
{
	delta_definition_list_t *next;
	char *ptypename;
	int numelements;
	delta_definition_t *pdefinition;
} delta_definition_list_t;

/* <23c69> ../engine/delta.c:67 */
typedef struct delta_registry_s
{
	delta_registry_t *next;
	char *name;
	delta_t *pdesc;
} delta_registry_t;


delta_definition_list_t *g_defs;
delta_encoder_t *g_encoders;
delta_registry_t *g_deltaregistry;

#endif // Defines_and_Variables_region

#ifndef Delta_definitions_region

#define DELTA_D_DEF(member) #member, offsetof(delta_description_s, member)
#define DELTA_DEF(structname, member) { #member, offsetof(structname, member) }

static delta_definition_t g_DeltaDataDefinition[] =
{
	DELTA_DEF(delta_description_s, fieldType),
	DELTA_DEF(delta_description_s, fieldName),
	DELTA_DEF(delta_description_s, fieldOffset),
	DELTA_DEF(delta_description_s, fieldSize),
	DELTA_DEF(delta_description_s, significant_bits),
	DELTA_DEF(delta_description_s, premultiply),
	DELTA_DEF(delta_description_s, postmultiply),
	DELTA_DEF(delta_description_s, flags),
};

static delta_description_t g_MetaDescription[] =
{
	{ DT_INTEGER, DELTA_D_DEF(fieldType), 1, 32, 1.0, 1.0, 0, 0, 0 },
	{ DT_STRING, DELTA_D_DEF(fieldName), 1, 1, 1.0, 1.0, 0, 0, 0 },
	{ DT_INTEGER, DELTA_D_DEF(fieldOffset), 1, 16, 1.0, 1.0, 0, 0, 0 },
	{ DT_INTEGER, DELTA_D_DEF(fieldSize), 1, 8, 1.0, 1.0, 0, 0, 0 },
	{ DT_INTEGER, DELTA_D_DEF(significant_bits), 1, 8, 1.0, 1.0, 0, 0, 0 },
	{ DT_FLOAT, DELTA_D_DEF(premultiply), 1, 32, 4000.0, 1.0, 0, 0, 0 },
	{ DT_FLOAT, DELTA_D_DEF(postmultiply), 1, 32, 4000.0, 1.0, 0, 0, 0 },
};

delta_t g_MetaDelta[] =
{
	{ 0, ARRAYSIZE(g_MetaDescription), "", NULL, g_MetaDescription },
};

static delta_definition_t g_EventDataDefinition[] =
{
	DELTA_DEF(event_args_s, entindex),
	DELTA_DEF(event_args_s, origin[0]),
	DELTA_DEF(event_args_s, origin[1]),
	DELTA_DEF(event_args_s, origin[2]),
	DELTA_DEF(event_args_s, angles[0]),
	DELTA_DEF(event_args_s, angles[1]),
	DELTA_DEF(event_args_s, angles[2]),
	DELTA_DEF(event_args_s, fparam1),
	DELTA_DEF(event_args_s, fparam2),
	DELTA_DEF(event_args_s, iparam1),
	DELTA_DEF(event_args_s, iparam2),
	DELTA_DEF(event_args_s, bparam1),
	DELTA_DEF(event_args_s, bparam2),
	DELTA_DEF(event_args_s, ducking),
};
static delta_definition_t g_EntityDataDefinition[] =
{
	DELTA_DEF(entity_state_s, startpos[0]),
	DELTA_DEF(entity_state_s, startpos[1]),
	DELTA_DEF(entity_state_s, startpos[2]),
	DELTA_DEF(entity_state_s, endpos[0]),
	DELTA_DEF(entity_state_s, endpos[1]),
	DELTA_DEF(entity_state_s, endpos[2]),
	DELTA_DEF(entity_state_s, impacttime),
	DELTA_DEF(entity_state_s, starttime),
	DELTA_DEF(entity_state_s, origin[0]),
	DELTA_DEF(entity_state_s, origin[1]),
	DELTA_DEF(entity_state_s, origin[2]),
	DELTA_DEF(entity_state_s, angles[0]),
	DELTA_DEF(entity_state_s, angles[1]),
	DELTA_DEF(entity_state_s, angles[2]),
	DELTA_DEF(entity_state_s, modelindex),
	DELTA_DEF(entity_state_s, frame),
	DELTA_DEF(entity_state_s, movetype),
	DELTA_DEF(entity_state_s, colormap),
	DELTA_DEF(entity_state_s, skin),
	DELTA_DEF(entity_state_s, solid),
	DELTA_DEF(entity_state_s, scale),
	DELTA_DEF(entity_state_s, effects),
	DELTA_DEF(entity_state_s, sequence),
	DELTA_DEF(entity_state_s, animtime),
	DELTA_DEF(entity_state_s, framerate),
	DELTA_DEF(entity_state_s, controller[0]),
	DELTA_DEF(entity_state_s, controller[1]),
	DELTA_DEF(entity_state_s, controller[2]),
	DELTA_DEF(entity_state_s, controller[3]),
	DELTA_DEF(entity_state_s, blending[0]),
	DELTA_DEF(entity_state_s, blending[1]),
	DELTA_DEF(entity_state_s, body),
	DELTA_DEF(entity_state_s, owner),
	DELTA_DEF(entity_state_s, rendermode),
	DELTA_DEF(entity_state_s, renderamt),
	DELTA_DEF(entity_state_s, renderfx),
	DELTA_DEF(entity_state_s, rendercolor.r),
	DELTA_DEF(entity_state_s, rendercolor.g),
	DELTA_DEF(entity_state_s, rendercolor.b),
	DELTA_DEF(entity_state_s, weaponmodel),
	DELTA_DEF(entity_state_s, gaitsequence),
	DELTA_DEF(entity_state_s, mins[0]),
	DELTA_DEF(entity_state_s, mins[1]),
	DELTA_DEF(entity_state_s, mins[2]),
	DELTA_DEF(entity_state_s, maxs[0]),
	DELTA_DEF(entity_state_s, maxs[1]),
	DELTA_DEF(entity_state_s, maxs[2]),
	DELTA_DEF(entity_state_s, aiment),
	DELTA_DEF(entity_state_s, basevelocity[0]),
	DELTA_DEF(entity_state_s, basevelocity[1]),
	DELTA_DEF(entity_state_s, basevelocity[2]),
	DELTA_DEF(entity_state_s, friction),
	DELTA_DEF(entity_state_s, gravity),
	DELTA_DEF(entity_state_s, spectator),
	DELTA_DEF(entity_state_s, velocity[0]),
	DELTA_DEF(entity_state_s, velocity[1]),
	DELTA_DEF(entity_state_s, velocity[2]),
	DELTA_DEF(entity_state_s, team),
	DELTA_DEF(entity_state_s, playerclass),
	DELTA_DEF(entity_state_s, health),
	DELTA_DEF(entity_state_s, usehull),
	DELTA_DEF(entity_state_s, oldbuttons),
	DELTA_DEF(entity_state_s, onground),
	DELTA_DEF(entity_state_s, iStepLeft),
	DELTA_DEF(entity_state_s, flFallVelocity),
	DELTA_DEF(entity_state_s, weaponanim),
	DELTA_DEF(entity_state_s, eflags),
	DELTA_DEF(entity_state_s, iuser1),
	DELTA_DEF(entity_state_s, iuser2),
	DELTA_DEF(entity_state_s, iuser3),
	DELTA_DEF(entity_state_s, iuser4),
	DELTA_DEF(entity_state_s, fuser1),
	DELTA_DEF(entity_state_s, fuser2),
	DELTA_DEF(entity_state_s, fuser3),
	DELTA_DEF(entity_state_s, fuser4),
	DELTA_DEF(entity_state_s, vuser1[0]),
	DELTA_DEF(entity_state_s, vuser1[1]),
	DELTA_DEF(entity_state_s, vuser1[2]),
	DELTA_DEF(entity_state_s, vuser2[0]),
	DELTA_DEF(entity_state_s, vuser2[1]),
	DELTA_DEF(entity_state_s, vuser2[2]),
	DELTA_DEF(entity_state_s, vuser3[0]),
	DELTA_DEF(entity_state_s, vuser3[1]),
	DELTA_DEF(entity_state_s, vuser3[2]),
	DELTA_DEF(entity_state_s, vuser4[0]),
	DELTA_DEF(entity_state_s, vuser4[1]),
	DELTA_DEF(entity_state_s, vuser4[2]),
};
static delta_definition_t g_UsercmdDataDefinition[] =
{
	DELTA_DEF(usercmd_s, lerp_msec),
	DELTA_DEF(usercmd_s, msec),
	DELTA_DEF(usercmd_s, lightlevel),
	DELTA_DEF(usercmd_s, viewangles[0]),
	DELTA_DEF(usercmd_s, viewangles[1]),
	DELTA_DEF(usercmd_s, viewangles[2]),
	DELTA_DEF(usercmd_s, buttons),
	DELTA_DEF(usercmd_s, forwardmove),
	DELTA_DEF(usercmd_s, sidemove),
	DELTA_DEF(usercmd_s, upmove),
	DELTA_DEF(usercmd_s, impulse),
	DELTA_DEF(usercmd_s, weaponselect),
	DELTA_DEF(usercmd_s, impact_index),
	DELTA_DEF(usercmd_s, impact_position[0]),
	DELTA_DEF(usercmd_s, impact_position[1]),
	DELTA_DEF(usercmd_s, impact_position[2]),
};
static delta_definition_t g_WeaponDataDefinition[] =
{
	DELTA_DEF(weapon_data_s, m_iId),
	DELTA_DEF(weapon_data_s, m_iClip),
	DELTA_DEF(weapon_data_s, m_flNextPrimaryAttack),
	DELTA_DEF(weapon_data_s, m_flNextSecondaryAttack),
	DELTA_DEF(weapon_data_s, m_flTimeWeaponIdle),
	DELTA_DEF(weapon_data_s, m_fInReload),
	DELTA_DEF(weapon_data_s, m_fInSpecialReload),
	DELTA_DEF(weapon_data_s, m_flNextReload),
	DELTA_DEF(weapon_data_s, m_flPumpTime),
	DELTA_DEF(weapon_data_s, m_fReloadTime),
	DELTA_DEF(weapon_data_s, m_fAimedDamage),
	DELTA_DEF(weapon_data_s, m_fNextAimBonus),
	DELTA_DEF(weapon_data_s, m_fInZoom),
	DELTA_DEF(weapon_data_s, m_iWeaponState),
	DELTA_DEF(weapon_data_s, iuser1),
	DELTA_DEF(weapon_data_s, iuser2),
	DELTA_DEF(weapon_data_s, iuser3),
	DELTA_DEF(weapon_data_s, iuser4),
	DELTA_DEF(weapon_data_s, fuser1),
	DELTA_DEF(weapon_data_s, fuser2),
	DELTA_DEF(weapon_data_s, fuser3),
	DELTA_DEF(weapon_data_s, fuser4),
};
static delta_definition_t g_ClientDataDefinition[] =
{
	DELTA_DEF(clientdata_s, origin[0]),
	DELTA_DEF(clientdata_s, origin[1]),
	DELTA_DEF(clientdata_s, origin[2]),
	DELTA_DEF(clientdata_s, velocity[0]),
	DELTA_DEF(clientdata_s, velocity[1]),
	DELTA_DEF(clientdata_s, velocity[2]),
	DELTA_DEF(clientdata_s, viewmodel),
	DELTA_DEF(clientdata_s, punchangle[0]),
	DELTA_DEF(clientdata_s, punchangle[1]),
	DELTA_DEF(clientdata_s, punchangle[2]),
	DELTA_DEF(clientdata_s, flags),
	DELTA_DEF(clientdata_s, waterlevel),
	DELTA_DEF(clientdata_s, watertype),
	DELTA_DEF(clientdata_s, view_ofs[0]),
	DELTA_DEF(clientdata_s, view_ofs[1]),
	DELTA_DEF(clientdata_s, view_ofs[2]),
	DELTA_DEF(clientdata_s, health),
	DELTA_DEF(clientdata_s, bInDuck),
	DELTA_DEF(clientdata_s, weapons),
	DELTA_DEF(clientdata_s, flTimeStepSound),
	DELTA_DEF(clientdata_s, flDuckTime),
	DELTA_DEF(clientdata_s, flSwimTime),
	DELTA_DEF(clientdata_s, waterjumptime),
	DELTA_DEF(clientdata_s, maxspeed),
	DELTA_DEF(clientdata_s, m_iId),
	DELTA_DEF(clientdata_s, ammo_nails),
	DELTA_DEF(clientdata_s, ammo_shells),
	DELTA_DEF(clientdata_s, ammo_cells),
	DELTA_DEF(clientdata_s, ammo_rockets),
	DELTA_DEF(clientdata_s, m_flNextAttack),
	DELTA_DEF(clientdata_s, physinfo),
	DELTA_DEF(clientdata_s, fov),
	DELTA_DEF(clientdata_s, weaponanim),
	DELTA_DEF(clientdata_s, tfstate),
	DELTA_DEF(clientdata_s, pushmsec),
	DELTA_DEF(clientdata_s, deadflag),
	DELTA_DEF(clientdata_s, iuser1),
	DELTA_DEF(clientdata_s, iuser2),
	DELTA_DEF(clientdata_s, iuser3),
	DELTA_DEF(clientdata_s, iuser4),
	DELTA_DEF(clientdata_s, fuser1),
	DELTA_DEF(clientdata_s, fuser2),
	DELTA_DEF(clientdata_s, fuser3),
	DELTA_DEF(clientdata_s, fuser4),
	DELTA_DEF(clientdata_s, vuser1[0]),
	DELTA_DEF(clientdata_s, vuser1[1]),
	DELTA_DEF(clientdata_s, vuser1[2]),
	DELTA_DEF(clientdata_s, vuser2[0]),
	DELTA_DEF(clientdata_s, vuser2[1]),
	DELTA_DEF(clientdata_s, vuser2[2]),
	DELTA_DEF(clientdata_s, vuser3[0]),
	DELTA_DEF(clientdata_s, vuser3[1]),
	DELTA_DEF(clientdata_s, vuser3[2]),
	DELTA_DEF(clientdata_s, vuser4[0]),
	DELTA_DEF(clientdata_s, vuser4[1]),
	DELTA_DEF(clientdata_s, vuser4[2]),
};

#endif


/* <23f5d> ../engine/delta.c:346 */
delta_description_t *DELTA_FindField(delta_t *pFields, const char *pszField)
{
	int fieldCount = pFields->fieldCount;
	delta_description_t *pitem = pFields->pdd;

	for (int i = 0; i < fieldCount; i++, pitem++)
	{
		if (!Q_stricmp(pitem->fieldName, pszField))
		{
			return pitem;
		}
	}

	Con_Printf(__FUNCTION__ ":  Warning, couldn't find %s\n", pszField);
	return NULL;
}

/* <23fd7> ../engine/delta.c:370 */
int EXT_FUNC DELTA_FindFieldIndex(struct delta_s *pFields, const char *fieldname)
{
	int fieldCount = pFields->fieldCount;
	delta_description_t *pitem = pFields->pdd;

	for (int i = 0; i < fieldCount; i++, pitem++)
	{
		if (!Q_stricmp(pitem->fieldName, fieldname))
		{
			return i;
		}
	}

	Con_Printf(__FUNCTION__ ":  Warning, couldn't find %s\n", fieldname);
	return -1;
}

/* <24032> ../engine/delta.c:393 */
void EXT_FUNC DELTA_SetField(struct delta_s *pFields, const char *fieldname)
{
#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
	int index = DELTA_FindFieldIndex(pFields, fieldname);

	if (index != -1)
		DELTA_SetFieldByIndex(pFields, index);
#else
	delta_description_t *pTest = DELTA_FindField(pFields, fieldname);

	if (pTest)
	{
		pTest->flags |= FDT_MARK;
	}
#endif
}

/* <240b2> ../engine/delta.c:411 */
void EXT_FUNC DELTA_UnsetField(struct delta_s *pFields, const char *fieldname)
{
#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
	int index = DELTA_FindFieldIndex(pFields, fieldname);

	if (index != -1)
		DELTA_UnsetFieldByIndex(pFields, index);
#else
	delta_description_t *pTest = DELTA_FindField(pFields, fieldname);

	if (pTest)
	{
		pTest->flags &= ~FDT_MARK;
	}
#endif
}

/* <24132> ../engine/delta.c:429 */
void EXT_FUNC DELTA_SetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
	DELTAJit_SetFieldByIndex(pFields, fieldNumber);
#else
	pFields->pdd[fieldNumber].flags |= FDT_MARK;
#endif
}

/* <2416a> ../engine/delta.c:441 */
void EXT_FUNC DELTA_UnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
	DELTAJit_UnsetFieldByIndex(pFields, fieldNumber);
#else
	pFields->pdd[fieldNumber].flags &= ~FDT_MARK;
#endif
}

/* <23cc4> ../engine/delta.c:453 */
void DELTA_ClearFlags(delta_t *pFields)
{
	int i;
	delta_description_t *pitem;
	for (i = 0, pitem = pFields->pdd; i < pFields->fieldCount; i++, pitem++)
	{
		pitem->flags = 0;
	}
}

/* <241d2> ../engine/delta.c:473 */
int DELTA_TestDelta(unsigned char *from, unsigned char *to, delta_t *pFields)
{
#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
	return DELTAJit_TestDelta(from, to, pFields);
#else
	int i;
	char *st1, *st2;
	delta_description_t *pTest;
	int fieldType;
	int fieldCount = pFields->fieldCount;
	int length;
	int different;
	int neededBits = 0;
	int highestBit = -1;

	for (i = 0, pTest = pFields->pdd; i < fieldCount; i++, pTest++)
	{
		different = FALSE;

		fieldType = pTest->fieldType & ~DT_SIGNED;
		switch (fieldType)
		{
		case DT_BYTE:
			different = from[pTest->fieldOffset] != to[pTest->fieldOffset];
			break;
		case DT_SHORT:
			different = *(uint16 *)&from[pTest->fieldOffset] != *(uint16 *)&to[pTest->fieldOffset];
			break;
		case DT_FLOAT:
		case DT_INTEGER:
		case DT_ANGLE:
			different = *(uint32 *)&from[pTest->fieldOffset] != *(uint32 *)&to[pTest->fieldOffset];
			break;
#ifdef REHLDS_FIXES
		// don't use multiplier when checking, to increase performance
		// check values binary like it does in jit
		case DT_TIMEWINDOW_8:
		case DT_TIMEWINDOW_BIG:
			different = (*(int32 *)&from[pTest->fieldOffset]) != (*(int32 *)&to[pTest->fieldOffset]);
			break;
#else
		case DT_TIMEWINDOW_8:
			different = (int32)(*(float *)&from[pTest->fieldOffset] * 100.0) != (int32)(*(float *)&to[pTest->fieldOffset] * 100.0);
			break;
		case DT_TIMEWINDOW_BIG:
			different = (int32)(*(float *)&from[pTest->fieldOffset] * 1000.0) != (int32)(*(float *)&to[pTest->fieldOffset] * 1000.0);
			break;
#endif
		case DT_STRING:
			st1 = (char*)&from[pTest->fieldOffset];
			st2 = (char*)&to[pTest->fieldOffset];
			if (!(!*st1 && !*st2 || *st1 && *st2 && !Q_stricmp(st1, st2)))	// Not sure why it is case insensitive, but it looks so
			{
#ifdef REHLDS_FIXES
				different = TRUE;
				length = Q_strlen(st2) * 8;
#else // REHLDS_FIXES
				length = Q_strlen(st2);
#endif // REHLDS_FIXES
			}
			break;
		default:
			Con_Printf(__FUNCTION__ ": Bad field type %i\n", fieldType);
			break;
		}

		if (different)
		{
			highestBit = i;
			neededBits += fieldType == DT_STRING ? length + 8 : pTest->significant_bits;
		}
	}

	if (highestBit != -1)
	{
		neededBits += highestBit / 8 * 8 + 8;
	}

	return neededBits;
#endif
}

/* <24309> ../engine/delta.c:602 */
int DELTA_CountSendFields(delta_t *pFields)
{
	int i, c;
	int fieldCount = pFields->fieldCount;
	delta_description_t *pitem;
	for (i = 0, c = 0, pitem = pFields->pdd; i < fieldCount; i++, pitem++)
	{
		if (pitem->flags & FDT_MARK)
		{
			c++;
			pitem->stats.sendcount++;
		}
	}
	return c;
}

/* <24382> ../engine/delta.c:629 */
void DELTA_MarkSendFields(unsigned char *from, unsigned char *to, delta_t *pFields)
{
	int i;
	char *st1, *st2;
	delta_description_t *pTest;
	int fieldType;
	int fieldCount = pFields->fieldCount;

	for (i = 0, pTest = pFields->pdd; i < fieldCount; i++, pTest++)
	{
		fieldType = pTest->fieldType & ~DT_SIGNED;
		switch (fieldType)
		{
		case DT_BYTE:
			if (from[pTest->fieldOffset] != to[pTest->fieldOffset])
				pTest->flags |= FDT_MARK;
			break;
		case DT_SHORT:
			if (*(uint16 *)&from[pTest->fieldOffset] != *(uint16 *)&to[pTest->fieldOffset])
				pTest->flags |= FDT_MARK;
			break;
		case DT_FLOAT:
		case DT_INTEGER:
		case DT_ANGLE:
			if (*(uint32 *)&from[pTest->fieldOffset] != *(uint32 *)&to[pTest->fieldOffset])
				pTest->flags |= FDT_MARK;
			break;

// don't use multiplier when checking, to increase performance
#ifdef REHLDS_FIXES
		case DT_TIMEWINDOW_8:
		case DT_TIMEWINDOW_BIG:
			if (*(uint32 *)&from[pTest->fieldOffset] != *(uint32 *)&to[pTest->fieldOffset])
				pTest->flags |= FDT_MARK;
			break;
#else
		case DT_TIMEWINDOW_8:
			if ((int32)(*(float *)&from[pTest->fieldOffset] * 100.0) != (int32)(*(float *)&to[pTest->fieldOffset] * 100.0))
				pTest->flags |= FDT_MARK;
			break;
		case DT_TIMEWINDOW_BIG:
			if ((int32)(*(float *)&from[pTest->fieldOffset] * 1000.0) != (int32)(*(float *)&to[pTest->fieldOffset] * 1000.0))
				pTest->flags |= FDT_MARK;
			break;
#endif
		case DT_STRING:
			st1 = (char*)&from[pTest->fieldOffset];
			st2 = (char*)&to[pTest->fieldOffset];
			if (!(!*st1 && !*st2 || *st1 && *st2 && !Q_stricmp(st1, st2)))	// Not sure why it is case insensitive, but it looks so
				pTest->flags |= FDT_MARK;
			break;
		default:
			Con_Printf(__FUNCTION__ ": Bad field type %i\n", fieldType);
			break;
		}
	}
	if (pFields->conditionalencode)
		pFields->conditionalencode(pFields, from, to);
}

/* <2448d> ../engine/delta.c:738 */
void DELTA_SetSendFlagBits(delta_t *pFields, int *bits, int *bytecount)
{
	delta_description_t *pTest;
	int i;
	int lastbit = -1;
	int fieldCount = pFields->fieldCount;

	Q_memset(bits, 0, 8);

	for (i = fieldCount - 1, pTest = &pFields->pdd[i]; i >= 0; i--, pTest--)
	{
		if (pTest->flags & FDT_MARK)
		{
			if (lastbit == -1)
				lastbit = i;
			bits[i > 31 ? 1 : 0] |= 1 << (i & 0x1F);
		}
	}

#ifdef REHLDS_FIXES
	// fix for bad bytecount when no fields are marked
	if (lastbit == -1) {
		*bytecount = 0;
		return;
	}
#endif

	*bytecount = (lastbit >> 3) + 1;
}

qboolean DELTA_IsFieldMarked(delta_t* pFields, int fieldNumber)
{
#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
	return DELTAJit_IsFieldMarked(pFields, fieldNumber);
#else 
	return pFields->pdd[fieldNumber].flags & FDT_MARK;
#endif
}

/* <2456d> ../engine/delta.c:782 */
void DELTA_WriteMarkedFields(unsigned char *from, unsigned char *to, delta_t *pFields)
{
	int i;
	delta_description_t *pTest;
	int fieldSign;
	int fieldType;

	float f2;
	int fieldCount = pFields->fieldCount;

	for (i = 0, pTest = pFields->pdd; i < fieldCount; i++, pTest++)
	{
#if defined (REHLDS_OPT_PEDANTIC) || defined (REHLDS_FIXES)
		if (!DELTA_IsFieldMarked(pFields, i))
			continue;
#else
		if (!(pTest->flags & FDT_MARK))
			continue;
#endif

		fieldSign = pTest->fieldType & DT_SIGNED;
		fieldType = pTest->fieldType & ~DT_SIGNED;
		switch (fieldType)
		{
		case DT_BYTE:
			if (fieldSign)
			{
				int8 si8 = *(int8 *)&to[pTest->fieldOffset];
				si8 = (int8)((double)si8 * pTest->premultiply);
				MSG_WriteSBits(si8, pTest->significant_bits);
			}
			else
			{
				uint8 i8 = *(uint8 *)&to[pTest->fieldOffset];
				i8 = (uint8)((double)i8 * pTest->premultiply);
				MSG_WriteBits(i8, pTest->significant_bits);
			}
			break;
		case DT_SHORT:
			if (fieldSign)
			{
				int16 si16 = *(int16 *)&to[pTest->fieldOffset];
				si16 = (int16)((double)si16 * pTest->premultiply);
				MSG_WriteSBits(si16, pTest->significant_bits);
			}
			else
			{
				uint16 i16 = *(uint16 *)&to[pTest->fieldOffset];
				i16 = (uint16)((double)i16 * pTest->premultiply);
				MSG_WriteBits(i16, pTest->significant_bits);
			}
			break;
		case DT_FLOAT:
		{
			double val = (double)(*(float *)&to[pTest->fieldOffset]) * pTest->premultiply;
			if (fieldSign)
			{
				MSG_WriteSBits((int32)val, pTest->significant_bits);
			}
			else
			{
				MSG_WriteBits((uint32)val, pTest->significant_bits);
			}
			break;
		}
		case DT_INTEGER:
		{
			if (fieldSign)
			{
				int32 signedInt = *(int32 *)&to[pTest->fieldOffset];
				if (pTest->premultiply < 0.9999 || pTest->premultiply > 1.0001)
				{
					signedInt = (int32)((double)signedInt * pTest->premultiply);
				}
				MSG_WriteSBits(signedInt, pTest->significant_bits);
			}
			else
			{
				uint32 unsignedInt = *(uint32 *)&to[pTest->fieldOffset];
				if (pTest->premultiply < 0.9999 || pTest->premultiply > 1.0001)
				{
					unsignedInt = (uint32)((double)unsignedInt * pTest->premultiply);
				}
				MSG_WriteBits(unsignedInt, pTest->significant_bits);
			}
			break;
		}
		case DT_ANGLE:
			f2 = *(float *)&to[pTest->fieldOffset];
			MSG_WriteBitAngle(f2, pTest->significant_bits);
			break;
		case DT_TIMEWINDOW_8:
		{
			f2 = *(float *)&to[pTest->fieldOffset];
			int32 twVal = (int)(g_psv.time * 100.0) - (int)(f2 * 100.0);
			MSG_WriteSBits(twVal, 8);
			break;
		}
		case DT_TIMEWINDOW_BIG:
		{
			f2 = *(float *)&to[pTest->fieldOffset];
			int32 twVal = (int)(g_psv.time * pTest->premultiply) - (int)(f2 * pTest->premultiply);
			MSG_WriteSBits((int32)twVal, pTest->significant_bits);
			break;
		}
		case DT_STRING:
			MSG_WriteBitString((const char *)&to[pTest->fieldOffset]);
			break;
		default:
			Con_Printf(__FUNCTION__ ": unknown send field type\n");
			break;
		}
	}
}

/* <2467e> ../engine/delta.c:924 */
qboolean DELTA_CheckDelta(unsigned char *from, unsigned char *to, delta_t *pFields)
{
	qboolean sendfields;

#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
	sendfields = DELTAJit_Fields_Clear_Mark_Check(from, to, pFields, NULL);
#else
	DELTA_ClearFlags(pFields);
	DELTA_MarkSendFields(from, to, pFields);
	sendfields = DELTA_CountSendFields(pFields);
#endif
	
	return sendfields;
}

/* <247f5> ../engine/delta.c:949 */
NOINLINE qboolean DELTA_WriteDelta(unsigned char *from, unsigned char *to, qboolean force, delta_t *pFields, void(*callback)(void))
{
	qboolean sendfields;

#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
	sendfields = DELTAJit_Fields_Clear_Mark_Check(from, to, pFields, NULL);
#else // REHLDS_OPT_PEDANTIC || REHLDS_FIXES
	DELTA_ClearFlags(pFields);
	DELTA_MarkSendFields(from, to, pFields);
	sendfields = DELTA_CountSendFields(pFields);
#endif // REHLDS_OPT_PEDANTIC || REHLDS_FIXES

	_DELTA_WriteDelta(from, to, force, pFields, callback, sendfields);
	return sendfields;
}

#ifdef REHLDS_FIXES //Fix for https://github.com/dreamstalker/rehlds/issues/24
qboolean DELTA_WriteDeltaForceMask(unsigned char *from, unsigned char *to, qboolean force, delta_t *pFields, void(*callback)(void), void* pForceMask) {
	qboolean sendfields = DELTAJit_Fields_Clear_Mark_Check(from, to, pFields, pForceMask);
	_DELTA_WriteDelta(from, to, force, pFields, callback, sendfields);
	return sendfields;
}
#endif


qboolean _DELTA_WriteDelta(unsigned char *from, unsigned char *to, qboolean force, delta_t *pFields, void(*callback)( void ), qboolean sendfields)
{
	int i;
	int bytecount;
	int bits[2];

	if (sendfields || force)
	{
#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
		DELTAJit_SetSendFlagBits(pFields, bits, &bytecount);
#else
		DELTA_SetSendFlagBits(pFields, bits, &bytecount);
#endif

		if (callback)
			callback();

		MSG_WriteBits(bytecount, 3);
		for (i = 0; i < bytecount; i++)
		{
			MSG_WriteBits(( (byte*)bits )[i], 8);
		}

		DELTA_WriteMarkedFields(from, to, pFields);
	}

	return 1;
}


/* <24aa0> ../engine/delta.c:1010 */
int DELTA_ParseDelta(unsigned char *from, unsigned char *to, delta_t *pFields)
{
	delta_description_t *pTest;
	int i;
	int bits[2];	// this is a limit with 64 fields max in delta
	int nbytes;
	int bitfieldnumber;
	int fieldCount = pFields->fieldCount;
	int fieldType;
	int fieldSign;

	double d2;
	float t;
	int addt;
	char *st2;
	char c;
	int startbit;

	startbit = MSG_CurrentBit();
	Q_memset(bits, 0, 8);

	nbytes = MSG_ReadBits(3);
	for (i = 0; i < nbytes; i++)
	{
		((byte*)bits)[i] = MSG_ReadBits(8);
	}

	for (i = 0, pTest = pFields->pdd; i < fieldCount; i++, pTest++)
	{
		fieldType = pTest->fieldType & ~DT_SIGNED;

		bitfieldnumber = 1 << (i & 0x1F);
		if (!(bitfieldnumber & bits[i > 31]))
		{
			// Field was not sent to us, just transfer info from the "from"
			switch (fieldType)
			{
			case DT_BYTE:
				to[pTest->fieldOffset] = from[pTest->fieldOffset];
				break;
			case DT_SHORT:
				*(uint16 *)&to[pTest->fieldOffset] = *(uint16 *)&from[pTest->fieldOffset];
				break;
			case DT_FLOAT:
			case DT_INTEGER:
			case DT_ANGLE:
			case DT_TIMEWINDOW_8:
			case DT_TIMEWINDOW_BIG:
				*(uint32 *)&to[pTest->fieldOffset] = *(uint32 *)&from[pTest->fieldOffset];
				break;
			case DT_STRING:
				Q_strcpy((char *)&to[pTest->fieldOffset], (char *)&from[pTest->fieldOffset]);
				break;
			default:
				Con_Printf(__FUNCTION__ ": unparseable field type %i\n", fieldType);
			}
			continue;
		}

		pTest->stats.receivedcount++;

		fieldSign = pTest->fieldType & DT_SIGNED;
		switch (fieldType)
		{
		case DT_BYTE:
			if (fieldSign)
			{
				d2 = (double)MSG_ReadSBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001)
				{
					d2 = d2 / pTest->premultiply;
				}
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001)
				{
					d2 = d2 * pTest->postmultiply;
				}
				*(int8 *)&to[pTest->fieldOffset] = (int8)d2;
			}
			else
			{
				d2 = (double)MSG_ReadBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001)
				{
					d2 = d2 / pTest->premultiply;
				}
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001)
				{
					d2 = d2 * pTest->postmultiply;
				}
				*(uint8 *)&to[pTest->fieldOffset] = (uint8)d2;
			}
			break;
		case DT_SHORT:
			if (fieldSign)
			{
				d2 = (double)MSG_ReadSBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001)
				{
					d2 = d2 / pTest->premultiply;
				}
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001)
				{
					d2 = d2 * pTest->postmultiply;
				}
				*(int16 *)&to[pTest->fieldOffset] = (int16)d2;
			}
			else
			{
				d2 = (double)MSG_ReadBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001)
				{
					d2 = d2 / pTest->premultiply;
				}
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001)
				{
					d2 = d2 * pTest->postmultiply;
				}
				*(uint16 *)&to[pTest->fieldOffset] = (uint16)d2;
			}
			break;
		case DT_FLOAT:
			if (fieldSign)
			{
				d2 = (double)MSG_ReadSBits(pTest->significant_bits);
			}
			else
			{
				d2 = (double)MSG_ReadBits(pTest->significant_bits);
			}
			if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001)
			{
				d2 = d2 / pTest->premultiply;
			}
			if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001)
			{
				d2 = d2 * pTest->postmultiply;
			}
			*(float *)&to[pTest->fieldOffset] = (float)d2;
			break;
		case DT_INTEGER:
			if (fieldSign)
			{
				d2 = (double)MSG_ReadSBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001)
				{
					d2 = d2 / pTest->premultiply;
				}
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001)
				{
					d2 = d2 * pTest->postmultiply;
				}
				*(int32 *)&to[pTest->fieldOffset] = (int32)d2;
			}
			else
			{
				d2 = (double)MSG_ReadBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001)
				{
					d2 = d2 / pTest->premultiply;
				}
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001)
				{
					d2 = d2 * pTest->postmultiply;
				}
				*(uint32 *)&to[pTest->fieldOffset] = (uint32)d2;
			}
			break;
		case DT_ANGLE:
			*(float *)&to[pTest->fieldOffset] = MSG_ReadBitAngle(pTest->significant_bits);
			break;
		case DT_TIMEWINDOW_8:
			addt = MSG_ReadSBits(8);
			t = (float)((g_pcl.mtime[0] * 100.0 - addt) / 100.0);
			*(float *)&to[pTest->fieldOffset] = t;
			break;
		case DT_TIMEWINDOW_BIG:
			addt = MSG_ReadSBits(pTest->significant_bits);
			if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001)
			{
				t = (float)((g_pcl.mtime[0] * pTest->premultiply - addt) / pTest->premultiply);
			}
			else
			{
				t = (float)(g_pcl.mtime[0] - addt);
			}
			*(float *)&to[pTest->fieldOffset] = t;
			break;
		case DT_STRING:
			st2 = (char *)&to[pTest->fieldOffset];
			do
			{
				c = MSG_ReadBits(8);
				*st2++ = c;
			} while (c);
			break;
		default:
			Con_Printf(__FUNCTION__ ": unparseable field type %i\n", fieldType);
			break;
		}
	}

	return MSG_CurrentBit() - startbit;
}

/* <24c50> ../engine/delta.c:1276 */
void EXT_FUNC DELTA_AddEncoder(char *name, void(*conditionalencode)(struct delta_s *, const unsigned char *, const unsigned char *))
{
	delta_encoder_t *p = (delta_encoder_t *)Mem_ZeroMalloc(sizeof(delta_encoder_t));
	p->name = Mem_Strdup(name);
	p->conditionalencode = conditionalencode;
	p->next = g_encoders;
	g_encoders = p;
}

/* <24c9d> ../engine/delta.c:1293 */
void DELTA_ClearEncoders(void)
{
	delta_encoder_t *n, *p = g_encoders;
	while (p)
	{
		n = p->next;
		Mem_Free(p->name);
		Mem_Free(p);
		p = n;
	}
	g_encoders = 0;
}

/* <24ce9> ../engine/delta.c:1315 */
encoder_t DELTA_LookupEncoder(char *name)
{
	delta_encoder_t *p = g_encoders;
	while (p)
	{
		if (!Q_stricmp(name, p->name))
		{
			return p->conditionalencode;
		}
		p = p->next;
	}
	return NULL;
}

/* <23d3e> ../engine/delta.c:1338 */
int DELTA_CountLinks(delta_link_t *plinks)
{
	delta_link_t *p = plinks;

	int c;
	for (c = 0; p != NULL; c++)
	{
		p = p->next;
	}

	return c;
}

/* <23cf6> ../engine/delta.c:1360 */
void DELTA_ReverseLinks(delta_link_t **plinks)
{
	delta_link_t *n, *p = *plinks;
	delta_link_t *newlist = NULL;

	while (p)
	{
		n = p->next;
		p->next = newlist;
		newlist = p;
		p = n;
	}

	*plinks = newlist;
}

/* <24d8d> ../engine/delta.c:1383 */
void DELTA_ClearLinks(delta_link_t **plinks)
{
	delta_link_t *n, *p = *plinks;
	while (p)
	{
		n = p->next;
		Mem_Free(p);
		p = n;
	}
	*plinks = 0;
}

/* <24ded> ../engine/delta.c:1405 */
delta_t *DELTA_BuildFromLinks(delta_link_t **pplinks)
{
	delta_description_t *pdesc, *pcur;
	delta_t *pdelta;
	delta_link_t *p;
	int count;

	pdelta = (delta_t *)Mem_ZeroMalloc(sizeof(delta_t));

	DELTA_ReverseLinks(pplinks);

	count = DELTA_CountLinks(*pplinks);

#ifdef REHLDS_FIXES
	if (count > DELTA_MAX_FIELDS)
		Sys_Error(__FUNCTION__ ": Too many fields in delta description %i (MAX %i)\n", count, DELTA_MAX_FIELDS);
#endif

	pdesc = (delta_description_t *)Mem_ZeroMalloc(sizeof(delta_description_t) * count);

	for (p = *pplinks, pcur = pdesc; p != NULL; p = p->next, pcur++)
	{
		Q_memcpy(pcur, p->delta, sizeof(delta_description_t));
		Mem_Free(p->delta);
		p->delta = 0;
	}

	DELTA_ClearLinks(pplinks);

	pdelta->dynamic = 1;
	pdelta->fieldCount = count;
	pdelta->pdd = pdesc;

	return pdelta;
}

/* <23dfa> ../engine/delta.c:1445 */
int DELTA_FindOffset(int count, delta_definition_t *pdef, char *fieldname)
{
	for (int i = 0; i < count; i++)
	{
		if (!Q_stricmp(fieldname, pdef[i].fieldName))
		{
			return pdef[i].fieldOffset;
		}
	}

	Sys_Error(__FUNCTION__ ": Couldn't find offset for %s!!!\n", fieldname);
}

/* <24f58> ../engine/delta.c:1469 */
qboolean DELTA_ParseType(delta_description_t *pdelta, char **pstream)
{
	// Read the stream till we hit the end
	while (*pstream = COM_Parse(*pstream), com_token[0] != 0)
	{
		if (!Q_stricmp(com_token, ","))
			return TRUE;	// end of type description

		if (!Q_stricmp(com_token, "|"))
			continue;	// skip | token

		// Determine field type
		if (!Q_stricmp(com_token, "DT_SIGNED"))
			pdelta->fieldType |= DT_SIGNED;
		else if (!Q_stricmp(com_token, "DT_BYTE"))
			pdelta->fieldType |= DT_BYTE;
		else if (!Q_stricmp(com_token, "DT_SHORT"))
			pdelta->fieldType |= DT_SHORT;
		else if (!Q_stricmp(com_token, "DT_FLOAT"))
			pdelta->fieldType |= DT_FLOAT;
		else if (!Q_stricmp(com_token, "DT_INTEGER"))
			pdelta->fieldType |= DT_INTEGER;
		else if (!Q_stricmp(com_token, "DT_ANGLE"))
			pdelta->fieldType |= DT_ANGLE;
		else if (!Q_stricmp(com_token, "DT_TIMEWINDOW_8"))
			pdelta->fieldType |= DT_TIMEWINDOW_8;
		else if (!Q_stricmp(com_token, "DT_TIMEWINDOW_BIG"))
			pdelta->fieldType |= DT_TIMEWINDOW_BIG;
		else if (!Q_stricmp(com_token, "DT_STRING"))
			pdelta->fieldType |= DT_STRING;
		else
			Sys_Error(__FUNCTION__ ":  Unknown type or type flag %s\n", com_token);
	}

	// We are hit the end of the stream
	Sys_Error(__FUNCTION__ ":  Expecting fieldtype info\n");	// Was Con_Printf here
	return FALSE;
}

/* <24f95> ../engine/delta.c:1545 */
qboolean DELTA_ParseField(int count, delta_definition_t *pdefinition, delta_link_t *pField, char **pstream)
{
	int readpost;

	readpost = 0;
	if (Q_stricmp(com_token, "DEFINE_DELTA"))
	{
		if (Q_stricmp(com_token, "DEFINE_DELTA_POST"))
		{
			Sys_Error(__FUNCTION__ ":  Expecting DEFINE_*, got %s\n", com_token);
		}
		readpost = 1;
	}

	*pstream = COM_Parse(*pstream);
	if (Q_stricmp(com_token, "("))
	{
		Sys_Error(__FUNCTION__ ":  Expecting (, got %s\n", com_token);
	}

	*pstream = COM_Parse(*pstream);
	if (com_token[0] == 0)
	{
		Sys_Error(__FUNCTION__ ":  Expecting fieldname\n");
	}

	Q_strncpy(pField->delta->fieldName, com_token, 31);
	pField->delta->fieldName[31] = 0;
	pField->delta->fieldOffset = DELTA_FindOffset(count, pdefinition, com_token);

	*pstream = COM_Parse(*pstream);
	if (!DELTA_ParseType(pField->delta, pstream))
	{
		return FALSE;
	}

	*pstream = COM_Parse(*pstream);
	pField->delta->fieldSize = 1;
	pField->delta->significant_bits = Q_atoi(com_token);
	*pstream = COM_Parse(*pstream);
	*pstream = COM_Parse(*pstream);
	pField->delta->premultiply = (float)Q_atof(com_token);

	if (readpost)
	{
		*pstream = COM_Parse(*pstream);
		*pstream = COM_Parse(*pstream);
		pField->delta->postmultiply = (float)Q_atof(com_token);
	}
	else
	{
		pField->delta->postmultiply = 1.0;
	}

	*pstream = COM_Parse(*pstream);
	if (Q_stricmp(com_token, ")"))
	{
		Sys_Error(__FUNCTION__ ":  Expecting ), got %s\n", com_token);	// Was Con_Printf here
		return FALSE;
	}

	*pstream = COM_Parse(*pstream);
	if (Q_stricmp(com_token, ","))
	{
		COM_UngetToken();
	}

	return TRUE;
}

/* <25051> ../engine/delta.c:1657 */
void DELTA_FreeDescription(delta_t **ppdesc)
{
	delta_t *p;

	if (ppdesc)
	{
		p = *ppdesc;
		if (p)
		{
			if (p->dynamic)
				Mem_Free(p->pdd);
			Mem_Free(p);
			*ppdesc = 0;
		}
	}
}

/* <23d92> ../engine/delta.c:1679 */
void DELTA_AddDefinition(char *name, delta_definition_t *pdef, int numelements)
{
	delta_definition_list_t *p = g_defs;
	while (p)
	{
		if (!Q_stricmp(name, p->ptypename))
		{
			break;
		}
		p = p->next;
	}

	if (p == NULL)
	{
		p = (delta_definition_list_t *)Mem_ZeroMalloc(sizeof(delta_definition_list_t));
		p->ptypename = Mem_Strdup(name);
		p->next = g_defs;
		g_defs = p;
	}

	p->pdefinition = pdef;
	p->numelements = numelements;
}

/* <25100> ../engine/delta.c:1713 */
void DELTA_ClearDefinitions(void)
{
	delta_definition_list_t *n, *p = g_defs;
	while (p)
	{
		n = p->next;
		Mem_Free(p->ptypename);
		Mem_Free(p);
		p = n;
	}
	g_defs = 0;
}

/* <23e3c> ../engine/delta.c:1735 */
delta_definition_t *DELTA_FindDefinition(char *name, int *count)
{
	delta_definition_list_t *p = g_defs;

	*count = 0;

	while (p)
	{
		if (!Q_stricmp(name, p->ptypename))
		{
			*count = p->numelements;
			return p->pdefinition;
		}
		p = p->next;
	}

	return NULL;
}

/* <2517b> ../engine/delta.c:1765 */
void DELTA_SkipDescription(char **pstream)
{
	*pstream = COM_Parse(*pstream);
	do
	{
		*pstream = COM_Parse(*pstream);
		if (com_token[0] == 0)
		{
			Sys_Error(__FUNCTION__ ": Error during description skip");
		}
	} while (Q_stricmp(com_token, "}"));
}

/* <251b5> ../engine/delta.c:1792 */
qboolean DELTA_ParseOneField(char **ppstream, delta_link_t **pplist, int count, delta_definition_t *pdefinition)
{
	delta_link_t *newlink;
	delta_link_t link;

	while (true)
	{
		if (!Q_stricmp(com_token, "}"))
		{
			COM_UngetToken();
			break;
		}

		*ppstream = COM_Parse(*ppstream);
		if (com_token[0] == 0)
		{
			break;
		}

		Q_memset(&link, 0, 8u);
		link.delta = (delta_description_t *)Mem_ZeroMalloc(sizeof(delta_description_t));
		if (!DELTA_ParseField(count, pdefinition, &link, ppstream))
		{
			return FALSE;
		}

		newlink = (delta_link_t *)Mem_ZeroMalloc(sizeof(delta_link_t));
		newlink->delta = link.delta;
		newlink->next = *pplist;
		*pplist = newlink;
	}
	return TRUE;
}

/* <2523f> ../engine/delta.c:1842 */
qboolean DELTA_ParseDescription(char *name, delta_t **ppdesc, char *pstream)
{
	delta_link_t *links;
	delta_definition_t *pdefinition;
	char encoder[32];
	char source[32];
	int count;


	links = NULL;
	count = 0;
	encoder[0] = 0;

	if (!ppdesc)
	{
		Sys_Error(__FUNCTION__ " with no delta_description_t\n");
	}
	*ppdesc = 0;

	if (!pstream)
	{
		Sys_Error(__FUNCTION__ " with no data stream\n");
	}

	while (true)
	{
		// Parse delta name
		pstream = COM_Parse(pstream);
		if (com_token[0] == 0)
		{
			break;
		}
		if (Q_stricmp(com_token, name))
		{
			DELTA_SkipDescription(&pstream);
		}
		else
		{
			pdefinition = DELTA_FindDefinition(com_token, &count);
			if (!pdefinition)
			{
				Sys_Error(__FUNCTION__ ":  Unknown data type:  %s\n", com_token);
			}

			// Parse source of conditional encoder
			pstream = COM_Parse(pstream);
			if (com_token[0] == 0)
			{
				Sys_Error(__FUNCTION__ ":  Unknown encoder :  %s\nValid values:\nnone\ngamedll funcname\nclientdll funcname\n", com_token);
			}
			if (Q_stricmp(com_token, "none"))
			{
				Q_strncpy(source, com_token, sizeof(source)-1);
				source[sizeof(source)-1] = 0;

				// Parse custom encoder function name
				pstream = COM_Parse(pstream);
				if (com_token[0] == 0)
				{
					Sys_Error(__FUNCTION__ ":  Expecting encoder\n");
				}

				Q_strncpy(encoder, com_token, sizeof(encoder)-1);
				encoder[sizeof(encoder)-1] = 0;
			}

			// Parse fields
			while (true)
			{
				pstream = COM_Parse(pstream);
				if (com_token[0] == 0)
				{
					break;
				}
				if (!Q_stricmp(com_token, "}"))
				{
					break;
				}
				if (Q_stricmp(com_token, "{"))
				{
					Sys_Error(__FUNCTION__ ":  Expecting {, got %s\n", com_token);	// Was Con_Printf here
					return FALSE;
				}
				if (!DELTA_ParseOneField(&pstream, &links, count, pdefinition))
				{
					return FALSE;
				}
			}
		}
	}

	*ppdesc = DELTA_BuildFromLinks(&links);

	if (encoder[0] != 0)
	{
		Q_strncpy((*ppdesc)->conditionalencodename, encoder, sizeof((*ppdesc)->conditionalencodename) - 1);
		(*ppdesc)->conditionalencodename[sizeof((*ppdesc)->conditionalencodename) - 1] = 0;
		(*ppdesc)->conditionalencode = 0;
	}

	return TRUE;
}

/* <25338> ../engine/delta.c:1959 */
qboolean DELTA_Load(char *name, delta_t **ppdesc, char *pszFile)
{
	char *pbuf;
	qboolean bret;

	pbuf = (char *)COM_LoadFile(pszFile, 5, 0);
	if (!pbuf)
	{
		Sys_Error(__FUNCTION__ ":  Couldn't load file %s\n", pszFile);
	}

	bret = DELTA_ParseDescription(name, ppdesc, pbuf);

	Mem_Free(pbuf);

	return bret;
}

/* <253a4> ../engine/delta.c:1990 */
void DELTA_RegisterDescription(char *name)
{
	delta_registry_t *p = (delta_registry_t *)Mem_ZeroMalloc(sizeof(delta_registry_t));
	p->next = g_deltaregistry;
	g_deltaregistry = p;
	p->name = Mem_Strdup(name);
	p->pdesc = 0;
}

/* <253dc> ../engine/delta.c:2008 */
void DELTA_ClearRegistrations(void)
{
	delta_registry_t *n, *p = g_deltaregistry;
	while (p)
	{
		n = p->next;
		Mem_Free(p->name);
		if (p->pdesc)
			DELTA_FreeDescription(&p->pdesc);
		Mem_Free(p);
		p = n;
	}
	g_deltaregistry = 0;
}

/* <25442> ../engine/delta.c:2036 */
delta_t **DELTA_LookupRegistration(const char *name)
{
	delta_registry_t *p = g_deltaregistry;
	while (p)
	{
		if (!Q_stricmp(p->name, name))
		{
			return &p->pdesc;
		}
		p = p->next;
	}
	return NULL;
}

/* <23dd6> ../engine/delta.c:2060 */
void DELTA_ClearStats(delta_t *p)
{
	int i;

	if (p)
	{
		for (i = p->fieldCount - 1; i >= 0; i--)
		{
			p->pdd[i].stats.sendcount = 0;
			p->pdd[i].stats.receivedcount = 0;
		}
	}
}

/* <23e72> ../engine/delta.c:2081 */
void DELTA_ClearStats_f(void)
{
	delta_registry_t *p;

	Con_Printf("Clearing delta stats\n");
	for (p = g_deltaregistry; p; p = p->next)
	{
		DELTA_ClearStats(p->pdesc);
	}
}

/* <23ece> ../engine/delta.c:2100 */
void DELTA_PrintStats(const char *name, delta_t *p)
{
	if (p)
	{
		Con_Printf("Stats for '%s'\n", name);
		if (p->fieldCount > 0)
		{
			delta_description_t *dt = p->pdd;
			for (int i = 0; i < p->fieldCount; i++, dt++)
			{
				Con_Printf("  %02i % 10s:  s % 5i r % 5i\n", i + 1, dt->fieldName, dt->stats.sendcount, dt->stats.receivedcount);
			}
		}
		Con_Printf("\n");
	}
}

/* <23efe> ../engine/delta.c:2124 */
void DELTA_DumpStats_f(void)
{
	Con_Printf("Delta Stats\n");
	for (delta_registry_t *dr = g_deltaregistry; dr; dr = dr->next)
		DELTA_PrintStats(dr->name, dr->pdesc);
}

/* <254d4> ../engine/delta.c:2143 */
void DELTA_Init(void)
{
	Cmd_AddCommand("delta_stats", DELTA_DumpStats_f);
	Cmd_AddCommand("delta_clear", DELTA_ClearStats_f);

	DELTA_AddDefinition("clientdata_t", g_ClientDataDefinition, ARRAYSIZE(g_ClientDataDefinition));
	DELTA_AddDefinition("weapon_data_t", g_WeaponDataDefinition, ARRAYSIZE(g_WeaponDataDefinition));
	DELTA_AddDefinition("usercmd_t", g_UsercmdDataDefinition, ARRAYSIZE(g_UsercmdDataDefinition));
	DELTA_AddDefinition("entity_state_t", g_EntityDataDefinition, ARRAYSIZE(g_EntityDataDefinition));
	DELTA_AddDefinition("entity_state_player_t", g_EntityDataDefinition, ARRAYSIZE(g_EntityDataDefinition));
	DELTA_AddDefinition("custom_entity_state_t", g_EntityDataDefinition, ARRAYSIZE(g_EntityDataDefinition));
	DELTA_AddDefinition("event_t", g_EventDataDefinition, ARRAYSIZE(g_EventDataDefinition));
}

/* <25847> ../engine/delta.c:2164 */
void DELTA_Shutdown(void)
{
	DELTA_ClearEncoders();
	DELTA_ClearDefinitions();
	DELTA_ClearRegistrations();
}
