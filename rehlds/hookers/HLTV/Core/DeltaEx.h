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

#pragma once

#ifdef HOOK_HLTV

#include "event_args.h"

#define g_defs					(*pg_defs)
#define g_encoders				(*pg_encoders)
#define g_deltaregistry			(*pg_deltaregistry)

#define g_pplayerdelta			(*pg_pplayerdelta)
#define g_pentitydelta			(*pg_pentitydelta)
#define g_pcustomentitydelta	(*pg_pcustomentitydelta)
#define g_pclientdelta			(*pg_pclientdelta)
#define g_pweapondelta			(*pg_pweapondelta)
#define g_peventdelta			(*pg_peventdelta)

#define g_delta_Time			(*pg_delta_Time)
#define g_large_Time_Buffers	(*pg_large_Time_Buffers)

typedef struct delta_link_s
{
	struct delta_link_s *next;
	delta_description_t *delta;
} delta_link_t;

typedef struct delta_definition_s
{
	char *fieldName;
	size_t fieldOffset;
} delta_definition_t;

typedef struct delta_definition_list_s
{
	struct delta_definition_list_s *next;
	char *ptypename;
	int numelements;
	delta_definition_t *pdefinition;
} delta_definition_list_t;

typedef struct delta_registry_s
{
	struct delta_registry_s *next;
	char *name;
	delta_t *pdesc;
} delta_registry_t;

namespace Delta {
	#define m_MetaDelta g_MetaDelta
	extern delta_t g_MetaDelta[];
};

#define m_EntityDelta g_pentitydelta
#define m_PlayerDelta g_pplayerdelta
#define m_CustomentityDelta g_pcustomentitydelta

#define m_ClientDelta g_pclientdelta
#define m_WeaponDelta g_pweapondelta
#define m_EventDelta g_peventdelta

extern delta_definition_list_t *g_defs;
extern delta_encoder_t *g_encoders;
extern delta_registry_t *g_deltaregistry;

extern delta_t *g_pentitydelta;
extern delta_t *g_pplayerdelta;
extern delta_t *g_pcustomentitydelta;
extern delta_t *g_pclientdelta;
extern delta_t *g_pweapondelta;
extern delta_t *g_peventdelta;

extern double g_delta_Time;
extern bool g_large_Time_Buffers;

delta_description_t *DELTA_FindField(delta_t *pFields, const char *pszField);
int DELTA_FindFieldIndex(delta_t *pFields, const char *fieldname);
void DELTA_SetField(delta_t *pFields, const char *fieldname);
void DELTA_UnsetField(delta_t *pFields, const char *fieldname);
void DELTA_SetFieldByIndex(delta_t *pFields, int fieldNumber);
void DELTA_UnsetFieldByIndex(delta_t *pFields, int fieldNumber);
void DELTA_ClearFlags(delta_t *pFields);
int DELTA_CountSendFields(delta_t *pFields);
void DELTA_MarkSendFields(unsigned char *from, unsigned char *to, delta_t *pFields);
void DELTA_SetSendFlagBits(delta_t *pFields, int *bits, int *bytecount);
void DELTA_WriteMarkedFields(BitBuffer *stream, unsigned char *from, unsigned char *to, delta_t *pFields);
int DELTA_CheckDelta(unsigned char *from, unsigned char *to, delta_t *pFields);
void DELTA_WriteHeader(BitBuffer *stream, deltacallback_t *header);
qboolean DELTA_WriteDelta(BitBuffer *stream, unsigned char *from, unsigned char *to, bool force, delta_t *pFields, deltacallback_t *header = nullptr);
int DELTA_ParseDelta(BitBuffer *stream, unsigned char *from, unsigned char *to, delta_t *pFields);
int DELTA_TestDelta(unsigned char *from, unsigned char *to, delta_t *pFields);
void DELTA_AddEncoder(char *name, encoder_t conditionalencode);
void DELTA_ClearEncoders();
encoder_t DELTA_LookupEncoder(char *name);
int DELTA_CountLinks(delta_link_t *plinks);
void DELTA_ReverseLinks(delta_link_t **plinks);
void DELTA_ClearLinks(delta_link_t **plinks);
delta_t *DELTA_BuildFromLinks(delta_link_t **pplinks);
int DELTA_FindOffset(int count, delta_definition_t *pdef, char *fieldname);
bool DELTA_ParseType(delta_description_t *pdelta, char **pstream);
bool DELTA_ParseField(int count, delta_definition_t *pdefinition, delta_link_t *pField, char **pstream);
void DELTA_FreeDescription(delta_t **ppdesc);
void DELTA_AddDefinition(char *name, delta_definition_t *pdef, int numelements);
void DELTA_ClearDefinitions();
delta_definition_t *DELTA_FindDefinition(char *name, int *count);
void DELTA_SkipDescription(char **pstream);
bool DELTA_ParseOneField(char **ppstream, delta_link_t **pplist, int count, delta_definition_t *pdefinition);
bool DELTA_ParseDescription(char *name, delta_t **ppdesc, char *pstream);
bool DELTA_Load(char *name, delta_t **ppdesc, char *pszFile);
void DELTA_RegisterDescription(char *name);
void DELTA_ClearRegistrations();
delta_t **DELTA_LookupRegistration(char *name);
void DELTA_ClearStats(delta_t *p);
void DELTA_Init();
void DELTA_UpdateDescriptions();
void DELTA_Shutdown();
void DELTA_SetTime(double time);
void DELTA_SetLargeTimeBufferSize(bool bigBuffers);

class DeltaWrapper {
public:
	void Init(IBaseSystem *system) { DELTA_Init(); }
	void Shutdown() { DELTA_Shutdown(); }
	void UpdateDescriptions() { DELTA_UpdateDescriptions(); }
	void WriteHeader(BitBuffer *stream, deltacallback_t *header) { DELTA_WriteHeader(stream, header); }
	bool WriteDelta(BitBuffer *stream, unsigned char *from, unsigned char *to, bool force, delta_t *pFields, deltacallback_t *header = nullptr) { return DELTA_WriteDelta(stream, from, to, force, pFields, header) ? true : false; }
	int ParseDelta(BitBuffer *stream, unsigned char *from, unsigned char *to, delta_t *pFields) { return DELTA_ParseDelta(stream, from, to, pFields); }
	void SetTime(double time) { DELTA_SetTime(time); }
	void SetLargeTimeBufferSize(bool bigBuffers) { DELTA_SetLargeTimeBufferSize(bigBuffers); }
	int TestDelta(unsigned char *from, unsigned char *to, delta_t *pFields) { return DELTA_TestDelta(from, to, pFields); }
	delta_t **LookupRegistration(char *name) { return DELTA_LookupRegistration(name); }
	void FreeDescription(delta_t **ppdesc) { DELTA_FreeDescription(ppdesc); }
	delta_registry_t *GetRegistry() const { return g_deltaregistry; }
};

#endif // HOOK_HLTV
