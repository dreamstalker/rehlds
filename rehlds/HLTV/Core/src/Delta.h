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

#include "event_args.h"
#include "BaseSystemModule.h"

#define DELTA_MAX_FIELDS	56			// 7*8

#define DT_BYTE				BIT(0)		// A byte
#define DT_SHORT			BIT(1)		// 2 byte field
#define DT_FLOAT			BIT(2)		// A floating point field
#define DT_INTEGER			BIT(3)		// 4 byte integer
#define DT_ANGLE			BIT(4)		// A floating point angle
#define DT_TIMEWINDOW_8		BIT(5)		// A floating point timestamp relative to server time
#define DT_TIMEWINDOW_BIG	BIT(6)		// A floating point timestamp relative to server time (with more precision and custom multiplier)
#define DT_STRING			BIT(7)		// A null terminated string, sent as 8 byte chars
#define DT_SIGNED			BIT(31)		// sign modificator

#define FDT_MARK			BIT(0)		// Delta mark for sending

typedef void (*encoder_t)(struct delta_s *, const unsigned char *, const unsigned char *);

typedef struct delta_encoder_s
{
	struct delta_encoder_s *next;
	char *name;
	encoder_t conditionalencode;
} delta_encoder_t;

typedef struct delta_stats_s
{
	int sendcount;
	int receivedcount;
} delta_stats_t;

typedef struct delta_description_s
{
	int fieldType;
	char fieldName[32];
	int fieldOffset;
	short int fieldSize;
	int significant_bits;
	float premultiply;
	float postmultiply;
	short int flags;
	delta_stats_t stats;
} delta_description_t;

typedef struct deltacallback_s
{
	int numbase;
	int num;
	bool remove;
	bool custom;
	bool newbl;
	int newblindex;
	bool full;
	int offset;
	bool instanced_baseline;
} deltacallback_t;

typedef struct delta_s
{
	int dynamic;
	int fieldCount;
	char conditionalencodename[32];
	encoder_t conditionalencode;
	delta_description_t *pdd;
} delta_t;

#include "hookers/HLTV/Core/DeltaEx.h"

#ifndef HOOK_HLTV

class Delta {
public:
	void Init(IBaseSystem *system);
	void Shutdown();
	void UpdateDescriptions();
	void WriteHeader(BitBuffer *stream, deltacallback_t *header);
	bool WriteDelta(BitBuffer *stream, unsigned char *from, unsigned char *to, bool force, delta_t *pFields, deltacallback_t *header = nullptr);
	int ParseDelta(BitBuffer *stream, unsigned char *from, unsigned char *to, delta_t *pFields);
	void SetTime(double time);
	void SetLargeTimeBufferSize(bool bigBuffers);
	int TestDelta(unsigned char *from, unsigned char *to, delta_t *pFields);
	delta_t **LookupRegistration(char *name);
	void FreeDescription(delta_t **ppdesc);

private:
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

	delta_registry_t *GetRegistry() const { return m_DeltaRegistry; }
	delta_description_t *FindField(delta_t *pFields, const char *pszField);
	int FindFieldIndex(delta_t *pFields, const char *fieldname);
	void SetField(delta_t *pFields, const char *fieldname);
	void UnsetField(delta_t *pFields, const char *fieldname);
	void SetFieldByIndex(delta_t *pFields, int fieldNumber);
	void UnsetFieldByIndex(delta_t *pFields, int fieldNumber);
	void ClearFlags(delta_t *pFields);
	int CountSendFields(delta_t *pFields);
	void MarkSendFields(unsigned char *from, unsigned char *to, delta_t *pFields);
	void SetSendFlagBits(delta_t *pFields, int *bits, int *bytecount);
	void WriteMarkedFields(BitBuffer *stream, unsigned char *from, unsigned char *to, delta_t *pFields);
	bool CheckDelta(unsigned char *from, unsigned char *to, delta_t *pFields);
	void AddEncoder(char *name, encoder_t conditionalencode);
	void ClearEncoders();
	encoder_t LookupEncoder(char *name);
	int CountLinks(delta_link_t *plinks);
	void ReverseLinks(delta_link_t **plinks);
	void ClearLinks(delta_link_t **plinks);
	delta_t *BuildFromLinks(delta_link_t **pplinks);
	int FindOffset(int count, delta_definition_t *pdef, char *fieldname);
	bool ParseType(delta_description_t *pdelta, char **pstream);
	bool ParseField(int count, delta_definition_t *pdefinition, delta_link_t *pField, char **pstream);
	void AddDefinition(char *name, delta_definition_t *pdef, int numelements);
	void ClearDefinitions();
	delta_definition_t *FindDefinition(char *name, int *count);
	void SkipDescription(char **pstream);
	bool ParseOneField(char **ppstream, delta_link_t **pplist, int count, delta_definition_t *pdefinition);
	bool ParseDescription(char *name, delta_t **ppdesc, char *pstream);
	bool Load(char *name, delta_t **ppdesc, char *pszFile);
	void RegisterDescription(char *name);
	void ClearRegistrations();
	void ClearStats(delta_t *p);

public:
	static delta_t m_MetaDelta[];
	static delta_description_t m_MetaDescription[];
	static delta_definition_t m_DeltaDataDefinition[];
	static delta_definition_t m_EventDataDefinition[];
	static delta_definition_t m_EntityDataDefinition[];
	static delta_definition_t m_UsercmdDataDefinition[];
	static delta_definition_t m_WeaponDataDefinition[];
	static delta_definition_t m_ClientDataDefinition[];

	static delta_t *m_EntityDelta;
	static delta_t *m_PlayerDelta;
	static delta_t *m_CustomentityDelta;

	static delta_t *m_ClientDelta;
	static delta_t *m_WeaponDelta;
	static delta_t *m_EventDelta;

private:
	friend class World;
	IBaseSystem *m_System;

	delta_definition_list_t *m_Defs;
	delta_encoder_t *m_Encoders;
	delta_registry_t *m_DeltaRegistry;

	double m_Time;
	bool m_LargeTime_Buffers;
};

#endif // HOOK_HLTV
