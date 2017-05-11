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

#ifdef HOOK_HLTV

DeltaWrapper World::m_Delta;

delta_definition_list_t *g_defs;
delta_encoder_t *g_encoders;
delta_registry_t *g_deltaregistry;

double g_delta_Time;
bool g_large_Time_Buffers;

delta_t *g_pentitydelta;
delta_t *g_pplayerdelta;
delta_t *g_pcustomentitydelta;
delta_t *g_pclientdelta;
delta_t *g_pweapondelta;
delta_t *g_peventdelta;

#define DELTA_D_DEF(member) #member, offsetof(delta_description_s, member)
#define DELTA_DEF(structname, member) { #member, offsetof(structname, member) }

delta_definition_t g_DeltaDataDefinition[] =
{
	DELTA_DEF(delta_description_s, fieldType),
	DELTA_DEF(delta_description_s, fieldName),
	DELTA_DEF(delta_description_s, fieldOffset),
	DELTA_DEF(delta_description_s, fieldSize),
	DELTA_DEF(delta_description_s, significant_bits),
	DELTA_DEF(delta_description_s, premultiply),
	DELTA_DEF(delta_description_s, postmultiply),
	DELTA_DEF(delta_description_s, flags)
};

delta_description_t g_MetaDescription[] =
{
	{ DT_INTEGER, DELTA_D_DEF(fieldType), 1, 32, 1.0, 1.0, 0, 0, 0 },
	{ DT_STRING,  DELTA_D_DEF(fieldName), 1, 1, 1.0, 1.0, 0, 0, 0 },
	{ DT_INTEGER, DELTA_D_DEF(fieldOffset), 1, 16, 1.0, 1.0, 0, 0, 0 },
	{ DT_INTEGER, DELTA_D_DEF(fieldSize), 1, 8, 1.0, 1.0, 0, 0, 0 },
	{ DT_INTEGER, DELTA_D_DEF(significant_bits), 1, 8, 1.0, 1.0, 0, 0, 0 },
	{ DT_FLOAT,   DELTA_D_DEF(premultiply), 1, 32, 4000.0, 1.0, 0, 0, 0 },
	{ DT_FLOAT,   DELTA_D_DEF(postmultiply), 1, 32, 4000.0, 1.0, 0, 0, 0 }
};

namespace Delta {
	delta_t g_MetaDelta[] =
	{
		{ 0, ARRAYSIZE(g_MetaDescription), "", NULL, g_MetaDescription },
	};

}; // namespace Delta

delta_definition_t g_EventDataDefinition[] =
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
	DELTA_DEF(event_args_s, ducking)
};

delta_definition_t g_EntityDataDefinition[] =
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
	DELTA_DEF(entity_state_s, vuser4[2])
};

delta_definition_t g_UsercmdDataDefinition[] =
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
	DELTA_DEF(usercmd_s, impact_position[2])
};

delta_definition_t g_WeaponDataDefinition[] =
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
	DELTA_DEF(weapon_data_s, fuser4)
};

delta_definition_t g_ClientDataDefinition[] =
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
	DELTA_DEF(clientdata_s, vuser4[2])
};

delta_description_t *DELTA_FindField(delta_t *pFields, const char *pszField)
{
	for (int i = 0; i < pFields->fieldCount; i++)
	{
		if (_stricmp(pFields->pdd[i].fieldName, pszField) == 0) {
			return &pFields->pdd[i];
		}
	}

	return nullptr;
}

int DELTA_FindFieldIndex(delta_t *pFields, const char *fieldname)
{
	for (int i = 0; i < pFields->fieldCount; i++)
	{
		if (_stricmp(pFields->pdd[i].fieldName, fieldname) == 0) {
			return i;
		}
	}

	return -1;
}

void DELTA_SetField(delta_t *pFields, const char *fieldname)
{
	delta_description_t *pTest = DELTA_FindField(pFields, fieldname);
	if (pTest) {
		pTest->flags |= FDT_MARK;
	}
}

void DELTA_UnsetField(delta_t *pFields, const char *fieldname)
{
	delta_description_t *pTest = DELTA_FindField(pFields, fieldname);
	if (pTest) {
		pTest->flags &= ~FDT_MARK;
	}
}

void DELTA_SetFieldByIndex(delta_t *pFields, int fieldNumber)
{
	pFields->pdd[fieldNumber].flags |= FDT_MARK;
}

void DELTA_UnsetFieldByIndex(delta_t *pFields, int fieldNumber)
{
	pFields->pdd[fieldNumber].flags &= ~FDT_MARK;
}

void DELTA_ClearFlags(delta_t *pFields)
{
	for (int i = 0; i < pFields->fieldCount; i++) {
		pFields->pdd[i].flags = 0;
	}
}

int DELTA_CountSendFields(delta_t *pFields)
{
	int i, c;
	for (i = 0, c = 0; i < pFields->fieldCount; i++)
	{
		auto pitem = &pFields->pdd[i];
		if (pitem->flags & FDT_MARK) {
			pitem->stats.sendcount++;
			c++;
		}
	}

	return c;
}

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
		case DT_TIMEWINDOW_8:
		case DT_TIMEWINDOW_BIG:
			if (*(uint32 *)&from[pTest->fieldOffset] != *(uint32 *)&to[pTest->fieldOffset])
				pTest->flags |= FDT_MARK;
			break;
		case DT_STRING:
			st1 = (char *)&from[pTest->fieldOffset];
			st2 = (char *)&to[pTest->fieldOffset];

			// Not sure why it is case insensitive, but it looks so
			if (!(!*st1 && !*st2 || *st1 && *st2 && !_stricmp(st1, st2))) {
				pTest->flags |= FDT_MARK;
			}
			break;
		default:
			break;
		}
	}

	if (pFields->conditionalencode) {
		pFields->conditionalencode(pFields, from, to);
	}
}

void DELTA_SetSendFlagBits(delta_t *pFields, int *bits, int *bytecount)
{
	int i;
	int lastbit = -1;
	int fieldCount = pFields->fieldCount;

	memset(bits, 0, 8);

	for (i = fieldCount - 1; i >= 0; i--)
	{
		if (pFields->pdd[i].flags & FDT_MARK)
		{
			if (lastbit == -1) {
				lastbit = i;
			}

			bits[i > 31 ? 1 : 0] |= 1 << (i & 0x1f);
		}
	}

	// fix for bad bytecount when no fields are marked
	if (lastbit == -1) {
		*bytecount = 0;
		return;
	}

	*bytecount = (lastbit >> 3) + 1;
}

void DELTA_WriteMarkedFields(BitBuffer *stream, unsigned char *from, unsigned char *to, delta_t *pFields)
{
	int i;
	delta_description_t *pTest;
	int fieldSign;
	int fieldType;

	float f2;
	int fieldCount = pFields->fieldCount;

	for (i = 0, pTest = pFields->pdd; i < fieldCount; i++, pTest++)
	{
		if (!(pTest->flags & FDT_MARK))
			continue;

		fieldSign = pTest->fieldType & DT_SIGNED;
		fieldType = pTest->fieldType & ~DT_SIGNED;
		switch (fieldType)
		{
		case DT_BYTE:
			if (fieldSign)
			{
				int8 si8 = *(int8 *)&to[pTest->fieldOffset];
				si8 = (int8)((double)si8 * pTest->premultiply);
				stream->WriteSBits(si8, pTest->significant_bits);
			}
			else
			{
				uint8 i8 = *(uint8 *)&to[pTest->fieldOffset];
				i8 = (uint8)((double)i8 * pTest->premultiply);
				stream->WriteBits(i8, pTest->significant_bits);
			}
			break;
		case DT_SHORT:
			if (fieldSign)
			{
				int16 si16 = *(int16 *)&to[pTest->fieldOffset];
				si16 = (int16)((double)si16 * pTest->premultiply);
				stream->WriteSBits(si16, pTest->significant_bits);
			}
			else
			{
				uint16 i16 = *(uint16 *)&to[pTest->fieldOffset];
				i16 = (uint16)((double)i16 * pTest->premultiply);
				stream->WriteBits(i16, pTest->significant_bits);
			}
			break;
		case DT_FLOAT:
		{
			double val = (double)(*(float *)&to[pTest->fieldOffset]) * pTest->premultiply;
			if (fieldSign)
			{
				stream->WriteSBits((int32)val, pTest->significant_bits);
			}
			else
			{
				stream->WriteBits((uint32)val, pTest->significant_bits);
			}
			break;
		}
		case DT_INTEGER:
		{
			if (fieldSign)
			{
				int32 signedInt = *(int32 *)&to[pTest->fieldOffset];
				if (pTest->premultiply < 0.9999 || pTest->premultiply > 1.0001) {
					signedInt = (int32)((double)signedInt * pTest->premultiply);
				}

				stream->WriteSBits(signedInt, pTest->significant_bits);
			}
			else
			{
				uint32 unsignedInt = *(uint32 *)&to[pTest->fieldOffset];
				if (pTest->premultiply < 0.9999 || pTest->premultiply > 1.0001) {
					unsignedInt = (uint32)((double)unsignedInt * pTest->premultiply);
				}

				stream->WriteBits(unsignedInt, pTest->significant_bits);
			}
			break;
		}
		case DT_ANGLE:
			f2 = *(float *)&to[pTest->fieldOffset];
			stream->WriteBitAngle(f2, pTest->significant_bits);
			break;
		case DT_TIMEWINDOW_8:
		{
			f2 = *(float *)&to[pTest->fieldOffset];
			if (g_large_Time_Buffers) {
				stream->WriteFloat(f2);
			} else {
				int32 twVal = (int)(g_delta_Time * 100.0) - (int)(f2 * 100.0);
				stream->WriteSBits(twVal, 8);
			}
			break;
		}
		case DT_TIMEWINDOW_BIG:
		{
			f2 = *(float *)&to[pTest->fieldOffset];
			if (g_large_Time_Buffers) {
				stream->WriteFloat(f2);
			} else {
				int32 twVal = (int)(g_delta_Time * pTest->premultiply) - (int)(f2 * pTest->premultiply);
				stream->WriteSBits((int32)twVal, pTest->significant_bits);
			}
			break;
		}
		case DT_STRING:
			stream->WriteBitString((const char *)&to[pTest->fieldOffset]);
			break;
		default:
			break;
		}
	}
}

int DELTA_CheckDelta(unsigned char *from, unsigned char *to, delta_t *pFields)
{
	DELTA_ClearFlags(pFields);
	DELTA_MarkSendFields(from, to, pFields);

	return DELTA_CountSendFields(pFields);
}

void DELTA_WriteHeader(BitBuffer *stream, deltacallback_t *header)
{
	int delta = header->num - header->numbase;
	if (header->full)
	{
		if (delta == 1)
		{
			stream->WriteBit(1);
		}
		else
		{
			stream->WriteBit(0);
			if (delta <= 0 || delta >= 64)
			{
				stream->WriteBit(1);
				stream->WriteBits(header->num, 11);
			}
			else
			{
				stream->WriteBit(0);
				stream->WriteBits(delta, 6);
			}
		}
	}
	else
	{
		stream->WriteBit(header->remove != 0);
		if (delta <= 0 || delta >= 64)
		{
			stream->WriteBit(1);
			stream->WriteBits(header->num, 11);
		}
		else
		{
			stream->WriteBit(0);
			stream->WriteBits(delta, 6);
		}
	}

	header->numbase = header->num;
	if (!header->remove)
	{
		stream->WriteBit(header->custom != 0);
		if (header->instanced_baseline)
		{
			if (header->newbl)
			{
				stream->WriteBit(1);
				stream->WriteBits(header->newblindex, 6);
			}
			else
			{
				stream->WriteBit(0);
			}
		}
		if (header->full && !header->newbl)
		{
			if (header->offset)
			{
				stream->WriteBit(1);
				stream->WriteBits(header->offset, 6);
			}
			else
			{
				stream->WriteBit(0);
			}
		}
	}
}

qboolean DELTA_WriteDelta(BitBuffer *stream, unsigned char *from, unsigned char *to, bool force, delta_t *pFields, deltacallback_t *header)
{
	int i;
	int bytecount;
	int bits[2];

	if (!DELTA_CheckDelta(from, to, pFields) && !force) {
		return FALSE;
	}

	DELTA_SetSendFlagBits(pFields, bits, &bytecount);

	if (header) {
		DELTA_WriteHeader(stream, header);
	}

	stream->WriteBits(bytecount, 3);
	for (i = 0; i < bytecount; i++) {
		stream->WriteBits(((byte *)bits)[i], 8);
	}

	DELTA_WriteMarkedFields(stream, from, to, pFields);
	return TRUE;
}

int DELTA_ParseDelta(BitBuffer *stream, unsigned char *from, unsigned char *to, delta_t *pFields)
{
	delta_description_t *pTest;
	int i;
	int bits[2]; // this is a limit with 64 fields max in delta
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

	startbit = stream->CurrentBit();
	memset(bits, 0, sizeof(bits));

	nbytes = stream->ReadBits(3);
	for (i = 0; i < nbytes; i++) {
		((byte *)bits)[i] = stream->ReadBits(8);
	}

	for (i = 0, pTest = pFields->pdd; i < fieldCount; i++, pTest++)
	{
		fieldType = pTest->fieldType & ~DT_SIGNED;

		bitfieldnumber = (1 << (i & 0x1F));
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
				strcpy((char *)&to[pTest->fieldOffset], (char *)&from[pTest->fieldOffset]);
				break;
			default:
				break;
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
				d2 = (double)stream->ReadSBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001) {
					d2 = d2 / pTest->premultiply;
				}
#if !defined(HLTV)
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001) {
					d2 = d2 * pTest->postmultiply;
				}
#endif
				*(int8 *)&to[pTest->fieldOffset] = (int8)d2;
			}
			else
			{
				d2 = (double)stream->ReadBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001) {
					d2 = d2 / pTest->premultiply;
				}
#if !defined(HLTV)
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001) {
					d2 = d2 * pTest->postmultiply;
				}
#endif
				*(uint8 *)&to[pTest->fieldOffset] = (uint8)d2;
			}
			break;
		case DT_SHORT:
			if (fieldSign)
			{
				d2 = (double)stream->ReadSBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001) {
					d2 = d2 / pTest->premultiply;
				}
#if !defined(HLTV)
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001) {
					d2 = d2 * pTest->postmultiply;
				}
#endif
				*(int16 *)&to[pTest->fieldOffset] = (int16)d2;
			}
			else
			{
				d2 = (double)stream->ReadBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001) {
					d2 = d2 / pTest->premultiply;
				}
#if !defined(HLTV)
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001) {
					d2 = d2 * pTest->postmultiply;
				}
#endif
				*(uint16 *)&to[pTest->fieldOffset] = (uint16)d2;
			}
			break;
		case DT_FLOAT:
			if (fieldSign)
			{
				d2 = (double)stream->ReadSBits(pTest->significant_bits);
			}
			else
			{
				d2 = (double)stream->ReadBits(pTest->significant_bits);
			}
			if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001) {
				d2 = d2 / pTest->premultiply;
			}
#if !defined(HLTV)
			if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001) {
				d2 = d2 * pTest->postmultiply;
			}
#endif
			*(float *)&to[pTest->fieldOffset] = (float)d2;
			break;
		case DT_INTEGER:
			if (fieldSign)
			{
				d2 = (double)stream->ReadSBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001) {
					d2 = d2 / pTest->premultiply;
				}
#if !defined(HLTV)
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001) {
					d2 = d2 * pTest->postmultiply;
				}
#endif
				*(int32 *)&to[pTest->fieldOffset] = (int32)d2;
			}
			else
			{
				d2 = (double)stream->ReadBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001) {
					d2 = d2 / pTest->premultiply;
				}
#if !defined(HLTV)
				if (pTest->postmultiply <= 0.9999 || pTest->postmultiply >= 1.0001) {
					d2 = d2 * pTest->postmultiply;
				}
#endif
				*(uint32 *)&to[pTest->fieldOffset] = (uint32)d2;
			}
			break;
		case DT_ANGLE:
			*(float *)&to[pTest->fieldOffset] = stream->ReadBitAngle(pTest->significant_bits);
			break;
		case DT_TIMEWINDOW_8:
			if (g_large_Time_Buffers) {
				t = stream->ReadFloat();
			}
			else {
				addt = stream->ReadSBits(8);
				t = (float)((g_delta_Time * 100.0 - addt) / 100.0);
			}
			*(float *)&to[pTest->fieldOffset] = t;
			break;
		case DT_TIMEWINDOW_BIG:
			if (g_large_Time_Buffers) {
				t = stream->ReadFloat();
			}
			else {
				addt = stream->ReadSBits(pTest->significant_bits);
				if (pTest->premultiply <= 0.9999 || pTest->premultiply >= 1.0001) {
					t = (float)((g_delta_Time * pTest->premultiply - addt) / pTest->premultiply);
				}
				else {
					t = (float)(g_delta_Time - addt);
				}
			}

			*(float *)&to[pTest->fieldOffset] = t;
			break;
		case DT_STRING:
			st2 = (char *)&to[pTest->fieldOffset];
			do
			{
				c = stream->ReadBits(8);
				*st2++ = c;
			} while (c);
			break;
		default:
			break;
		}
	}

	return stream->CurrentBit() - startbit;
}

int DELTA_TestDelta(unsigned char *from, unsigned char *to, delta_t *pFields)
{
	int i;
	char *st1, *st2;
	delta_description_t *pTest;
	int fieldType;
	int fieldCount = pFields->fieldCount;
	int length = 0;
	bool different;
	int neededBits = 0;
	int highestBit = -1;

	for (i = 0, pTest = pFields->pdd; i < fieldCount; i++, pTest++)
	{
		different = false;
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
		// don't use multiplier when checking, to increase performance
		// check values binary like it does in jit
		case DT_TIMEWINDOW_8:
		case DT_TIMEWINDOW_BIG:
			different = (*(int32 *)&from[pTest->fieldOffset]) != (*(int32 *)&to[pTest->fieldOffset]);
			break;
		case DT_STRING:
			st1 = (char *)&from[pTest->fieldOffset];
			st2 = (char *)&to[pTest->fieldOffset];

			// Not sure why it is case insensitive, but it looks so
			if (!(!*st1 && !*st2 || *st1 && *st2 && !_stricmp(st1, st2)))
			{
				different = true;
				length = strlen(st2) * 8;
				pTest->flags |= FDT_MARK;
			}
			break;
		default:
			break;
		}

		if (different)
		{
			highestBit = i;
			neededBits += (fieldType == DT_STRING) ? length + 8 : pTest->significant_bits;
		}
	}

	if (highestBit != -1) {
		neededBits += highestBit / 8 * 8 + 8;
	}

	return neededBits;
}

void DELTA_AddEncoder(char *name, encoder_t conditionalencode)
{
	delta_encoder_t *delta = (delta_encoder_t *)Mem_ZeroMalloc(sizeof(delta_encoder_t));
	delta->name = _strdup(name);
	delta->conditionalencode = conditionalencode;
	delta->next = g_encoders;
	g_encoders = delta;
}

void DELTA_ClearEncoders()
{
	delta_encoder_t *n, *p = g_encoders;
	while (p)
	{
		n = p->next;
		Mem_Free(p->name);
		Mem_Free(p);
		p = n;
	}

	g_encoders = nullptr;
}

encoder_t DELTA_LookupEncoder(char *name)
{
	delta_encoder_t *p = g_encoders;
	while (p)
	{
		if (_stricmp(name, p->name) == 0) {
			return p->conditionalencode;
		}

		p = p->next;
	}

	return nullptr;
}

int DELTA_CountLinks(delta_link_t *plinks)
{
	delta_link_t *p = plinks;

	int c;
	for (c = 0; p; c++) {
		p = p->next;
	}

	return c;
}

void DELTA_ReverseLinks(delta_link_t **plinks)
{
	delta_link_t *n, *p = *plinks;
	delta_link_t *newlist = nullptr;

	while (p)
	{
		n = p->next;
		p->next = newlist;
		newlist = p;
		p = n;
	}

	*plinks = newlist;
}

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

delta_t *DELTA_BuildFromLinks(delta_link_t **pplinks)
{
	delta_description_t *pdesc, *pcur;
	delta_t *pdelta;
	delta_link_t *p;
	int count;

	pdelta = (delta_t *)Mem_ZeroMalloc(sizeof(delta_t));
	DELTA_ReverseLinks(pplinks);
	count = DELTA_CountLinks(*pplinks);

	if (count > DELTA_MAX_FIELDS) {
		return nullptr;
	}

	pdesc = (delta_description_t *)Mem_ZeroMalloc(sizeof(delta_description_t) * count);

	for (p = *pplinks, pcur = pdesc; p; p = p->next, pcur++)
	{
		memcpy(pcur, p->delta, sizeof(delta_description_t));
		Mem_Free(p->delta);
		p->delta = nullptr;
	}

	DELTA_ClearLinks(pplinks);

	pdelta->dynamic = 1;
	pdelta->fieldCount = count;
	pdelta->pdd = pdesc;

	return pdelta;
}

int DELTA_FindOffset(int count, delta_definition_t *pdef, char *fieldname)
{
	for (int i = 0; i < count; i++)
	{
		if (_stricmp(fieldname, pdef[i].fieldName) == 0) {
			return pdef[i].fieldOffset;
		}
	}

	return 0;
}

bool DELTA_ParseType(delta_description_t *pdelta, char **pstream)
{
	// Read the stream till we hit the end
	while (*pstream = COM_Parse(*pstream), com_token[0] != 0)
	{
		if (!_stricmp(com_token, ","))
			return true;	// end of type description

		if (!_stricmp(com_token, "|"))
			continue;	// skip | token

		// Determine field type
		if (!_stricmp(com_token, "DT_SIGNED"))
			pdelta->fieldType |= DT_SIGNED;
		else if (!_stricmp(com_token, "DT_BYTE"))
			pdelta->fieldType |= DT_BYTE;
		else if (!_stricmp(com_token, "DT_SHORT"))
			pdelta->fieldType |= DT_SHORT;
		else if (!_stricmp(com_token, "DT_FLOAT"))
			pdelta->fieldType |= DT_FLOAT;
		else if (!_stricmp(com_token, "DT_INTEGER"))
			pdelta->fieldType |= DT_INTEGER;
		else if (!_stricmp(com_token, "DT_ANGLE"))
			pdelta->fieldType |= DT_ANGLE;
		else if (!_stricmp(com_token, "DT_TIMEWINDOW_8"))
			pdelta->fieldType |= DT_TIMEWINDOW_8;
		else if (!_stricmp(com_token, "DT_TIMEWINDOW_BIG"))
			pdelta->fieldType |= DT_TIMEWINDOW_BIG;
		else if (!_stricmp(com_token, "DT_STRING"))
			pdelta->fieldType |= DT_STRING;
		else
		{
			return false;
		}
	}

	// We are hit the end of the stream
	return false;
}

bool DELTA_ParseField(int count, delta_definition_t *pdefinition, delta_link_t *pField, char **pstream)
{
	bool readpost = false;
	if (_stricmp(com_token, "DEFINE_DELTA"))
	{
		if (_stricmp(com_token, "DEFINE_DELTA_POST") != 0) {
			return false;
		}

		readpost = true;
	}

	*pstream = COM_Parse(*pstream);
	if (_stricmp(com_token, "("))
	{
		return false;
	}

	*pstream = COM_Parse(*pstream);
	if (com_token[0] == '\0')
	{
		return false;
	}

	strncpy(pField->delta->fieldName, com_token, sizeof(pField->delta->fieldName) - 1);
	pField->delta->fieldName[sizeof(pField->delta->fieldName) - 1] = '\0';

	pField->delta->fieldOffset = DELTA_FindOffset(count, pdefinition, com_token);

	*pstream = COM_Parse(*pstream);
	if (!DELTA_ParseType(pField->delta, pstream)) {
		return false;
	}

	*pstream = COM_Parse(*pstream);
	pField->delta->fieldSize = 1;
	pField->delta->significant_bits = atoi(com_token);
	*pstream = COM_Parse(*pstream);
	*pstream = COM_Parse(*pstream);
	pField->delta->premultiply = (float)atof(com_token);

	if (readpost)
	{
		*pstream = COM_Parse(*pstream);
		*pstream = COM_Parse(*pstream);
		pField->delta->postmultiply = (float)atof(com_token);
	}
	else
	{
		pField->delta->postmultiply = 1.0;
	}

	*pstream = COM_Parse(*pstream);
	if (_stricmp(com_token, ")"))
	{
		return false;
	}

	*pstream = COM_Parse(*pstream);
	if (_stricmp(com_token, ",")) {
		COM_UngetToken();
	}

	return true;
}

void DELTA_FreeDescription(delta_t **ppdesc)
{
	if (ppdesc && *ppdesc)
	{
		if ((*ppdesc)->dynamic) {
			Mem_Free((*ppdesc)->pdd);
		}

		Mem_Free(*ppdesc);
		*ppdesc = nullptr;
	}
}

void DELTA_AddDefinition(char *name, delta_definition_t *pdef, int numelements)
{
	delta_definition_list_t *p = g_defs;
	while (p)
	{
		if (_stricmp(name, p->ptypename) == 0) {
			break;
		}

		p = p->next;
	}

	if (!p)
	{
		p = (delta_definition_list_t *)Mem_ZeroMalloc(sizeof(delta_definition_list_t));
		p->ptypename = _strdup(name);
		p->next = g_defs;
		g_defs = p;
	}

	p->pdefinition = pdef;
	p->numelements = numelements;
}

void DELTA_ClearDefinitions()
{
	delta_definition_list_t *n, *p = g_defs;
	while (p)
	{
		n = p->next;
		Mem_Free(p->ptypename);
		Mem_Free(p);
		p = n;
	}

	g_defs = nullptr;
}

delta_definition_t *DELTA_FindDefinition(char *name, int *count)
{
	*count = 0;

	delta_definition_list_t *p = g_defs;
	while (p)
	{
		if (!_stricmp(name, p->ptypename))
		{
			*count = p->numelements;
			return p->pdefinition;
		}

		p = p->next;
	}

	return nullptr;
}

void DELTA_SkipDescription(char **pstream)
{
	*pstream = COM_Parse(*pstream);
	do
	{
		*pstream = COM_Parse(*pstream);
		if (com_token[0] == '\0') {
			return;
		}
	}
	while (_stricmp(com_token, "}"));
}

bool DELTA_ParseOneField(char **ppstream, delta_link_t **pplist, int count, delta_definition_t *pdefinition)
{
	delta_link_t *newlink;
	delta_link_t link;

	while (true)
	{
		if (!_stricmp(com_token, "}"))
		{
			COM_UngetToken();
			break;
		}

		*ppstream = COM_Parse(*ppstream);
		if (com_token[0] == '\0') {
			break;
		}

		memset(&link, 0, sizeof(link));
		link.delta = (delta_description_t *)Mem_ZeroMalloc(sizeof(delta_description_t));
		if (!DELTA_ParseField(count, pdefinition, &link, ppstream)) {
			return false;
		}

		newlink = (delta_link_t *)Mem_ZeroMalloc(sizeof(delta_link_t));
		newlink->delta = link.delta;
		newlink->next = *pplist;
		*pplist = newlink;
	}

	return true;
}

bool DELTA_ParseDescription(char *name, delta_t **ppdesc, char *pstream)
{
	delta_link_t *links = nullptr;
	delta_definition_t *pdefinition;
	char encoder[32] = "";
	char source[32];
	int count = 0;

	if (!ppdesc) {
		return false;
	}

	*ppdesc = nullptr;

	if (!pstream) {
		return false;
	}

	while (true)
	{
		// Parse delta name
		pstream = COM_Parse(pstream);
		if (com_token[0] == '\0') {
			break;
		}

		if (_stricmp(com_token, name))
		{
			DELTA_SkipDescription(&pstream);
		}
		else
		{
			pdefinition = DELTA_FindDefinition(com_token, &count);
			if (!pdefinition) {
				return false;
			}

			// Parse source of conditional encoder
			pstream = COM_Parse(pstream);
			if (com_token[0] == '\0') {
				return false;
			}

			if (_stricmp(com_token, "none"))
			{
				strncpy(source, com_token, sizeof(source) - 1);
				source[sizeof(source) - 1] = '\0';

				// Parse custom encoder function name
				pstream = COM_Parse(pstream);
				if (com_token[0] == '\0') {
					return false;
				}

				strncpy(encoder, com_token, sizeof(encoder) - 1);
				encoder[sizeof(encoder) - 1] = '\0';
			}

			// Parse fields
			while (true)
			{
				pstream = COM_Parse(pstream);
				if (com_token[0] == '\0') {
					break;
				}

				if (!_stricmp(com_token, "}")) {
					break;
				}

				if (_stricmp(com_token, "{")) {
					return false;
				}

				if (!DELTA_ParseOneField(&pstream, &links, count, pdefinition)) {
					return false;
				}
			}
		}
	}

	*ppdesc = DELTA_BuildFromLinks(&links);

	if (encoder[0])
	{
		strncpy((*ppdesc)->conditionalencodename, encoder, sizeof((*ppdesc)->conditionalencodename) - 1);
		(*ppdesc)->conditionalencodename[sizeof((*ppdesc)->conditionalencodename) - 1] = '\0';
		(*ppdesc)->conditionalencode = nullptr;
	}

	return true;
}

bool DELTA_Load(char *name, delta_t **ppdesc, char *pszFile)
{
	return false;
}

void DELTA_RegisterDescription(char *name)
{
	delta_registry_t *p = (delta_registry_t *)Mem_ZeroMalloc(sizeof(delta_registry_t));
	p->next = g_deltaregistry;
	g_deltaregistry = p;
	p->name = _strdup(name);
	p->pdesc = 0;
}

void DELTA_ClearRegistrations()
{
	delta_registry_t *n, *p = g_deltaregistry;
	while (p)
	{
		n = p->next;
		Mem_Free(p->name);

		if (p->pdesc) {
			DELTA_FreeDescription(&p->pdesc);
		}

		Mem_Free(p);
		p = n;
	}

	g_deltaregistry = nullptr;
}

delta_t **DELTA_LookupRegistration(char *name)
{
	delta_registry_t *delta = g_deltaregistry;
	while (delta)
	{
		if (_stricmp(delta->name, name) == 0) {
			return &delta->pdesc;
		}

		delta = delta->next;
	}

	return nullptr;
}

void DELTA_ClearStats(delta_t *p)
{
	if (!p) {
		return;
	}

	for (int i = 0; i < p->fieldCount; i++)
	{
		p->pdd[i].stats.sendcount = 0;
		p->pdd[i].stats.receivedcount = 0;
	}
}

void DELTA_Init()
{
	g_defs = nullptr;
	g_encoders = nullptr;
	g_deltaregistry = nullptr;

	DELTA_AddDefinition("clientdata_t",          g_ClientDataDefinition,  ARRAYSIZE(g_ClientDataDefinition));
	DELTA_AddDefinition("weapon_data_t",         g_WeaponDataDefinition,  ARRAYSIZE(g_WeaponDataDefinition));
	DELTA_AddDefinition("usercmd_t",             g_UsercmdDataDefinition, ARRAYSIZE(g_UsercmdDataDefinition));
	DELTA_AddDefinition("entity_state_t",        g_EntityDataDefinition,  ARRAYSIZE(g_EntityDataDefinition));
	DELTA_AddDefinition("entity_state_player_t", g_EntityDataDefinition,  ARRAYSIZE(g_EntityDataDefinition));
	DELTA_AddDefinition("custom_entity_state_t", g_EntityDataDefinition,  ARRAYSIZE(g_EntityDataDefinition));
	DELTA_AddDefinition("event_t",               g_EventDataDefinition,   ARRAYSIZE(g_EventDataDefinition));

	DELTA_RegisterDescription("clientdata_t");
	DELTA_RegisterDescription("entity_state_t");
	DELTA_RegisterDescription("entity_state_player_t");
	DELTA_RegisterDescription("custom_entity_state_t");
	DELTA_RegisterDescription("usercmd_t");
	DELTA_RegisterDescription("weapon_data_t");
	DELTA_RegisterDescription("event_t");

	g_large_Time_Buffers = false;
}

void DELTA_UpdateDescriptions()
{
	g_pplayerdelta       = *DELTA_LookupRegistration("entity_state_player_t");
	g_pentitydelta       = *DELTA_LookupRegistration("entity_state_t");
	g_pcustomentitydelta = *DELTA_LookupRegistration("custom_entity_state_t");
	g_pclientdelta       = *DELTA_LookupRegistration("clientdata_t");
	g_pweapondelta       = *DELTA_LookupRegistration("weapon_data_t");
	g_peventdelta        = *DELTA_LookupRegistration("event_t");
}

void DELTA_Shutdown()
{
	DELTA_ClearEncoders();
	DELTA_ClearDefinitions();
	DELTA_ClearRegistrations();
}

void DELTA_SetTime(double time)
{
	g_delta_Time = time;
}

void DELTA_SetLargeTimeBufferSize(bool bigBuffers)
{
	g_large_Time_Buffers = bigBuffers;
}

bool World::IsDeltaEncoder() const
{
	if (g_pcustomentitydelta
		&& g_pentitydelta
		&& g_pplayerdelta) {
		return true;
	}

	return false;
}

delta_t *World::GetDeltaEncoder(int index, bool custom)
{
	if (custom) {
		return (delta_t *)g_pcustomentitydelta;
	}
	else if (IsPlayerIndex(index)) {
		return (delta_t *)g_pplayerdelta;
	}

	return  (delta_t *)g_pentitydelta;
}

delta_t *World::GetEventDelta() const {
	return g_peventdelta;
}

delta_t *World::GetClientDelta() const {
	return g_pclientdelta;
}

delta_t *World::GetEntityDelta() const {
	return g_pentitydelta;
}

delta_t *World::GetWeaponDelta() const {
	return g_pweapondelta;
}

#endif // HOOK_HLTV
