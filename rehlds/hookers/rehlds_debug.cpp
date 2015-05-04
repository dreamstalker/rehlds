#include "precompiled.h"

#define REHLDS_DEBUG_MAX_EDICTS 2048

uint32_t calcFloatChecksum(uint32_t crc, const float* pFloat) {
	uint32_t sVal = *reinterpret_cast<const uint32_t*>(pFloat);
	return _mm_crc32_u32(crc, sVal);
}

uint32_t calcVec3Checksum(uint32_t crc, const vec_t* v)
{
	crc = calcFloatChecksum(crc, v);
	crc = calcFloatChecksum(crc, v + 1);
	crc = calcFloatChecksum(crc, v + 2);
	return crc;
}

uint32_t calcEdictRefChecksum(uint32_t crc, edict_t* ed) {
	if (ed == NULL) {
		return _mm_crc32_u32(crc, -1);
	}

	return _mm_crc32_u32(crc, ed - g_psv.edicts);
}

uint32_t calcEngStringChecksum(uint32_t crc, int str) {
	if (str == 0) {
		return _mm_crc32_u8(crc, 0);
	}

	crc = _mm_crc32_u8(crc, 1);
	const char* cc = pr_strings + str;
	while (*cc) {
		crc = _mm_crc32_u8(crc, *cc);
		++cc;
	}

	crc = _mm_crc32_u8(crc, 0);
	return crc;
}

uint32_t calcEntvarsChecksum(uint32_t crc, const entvars_t* ev) {
	crc = calcEngStringChecksum(crc, ev->classname);
	crc = calcEngStringChecksum(crc, ev->globalname);

	crc = calcVec3Checksum(crc, ev->origin);
	crc = calcVec3Checksum(crc, ev->oldorigin);
	crc = calcVec3Checksum(crc, ev->velocity);
	crc = calcVec3Checksum(crc, ev->basevelocity);
	crc = calcVec3Checksum(crc, ev->clbasevelocity);
	crc = calcVec3Checksum(crc, ev->movedir);
	crc = calcVec3Checksum(crc, ev->angles);
	crc = calcVec3Checksum(crc, ev->avelocity);
	crc = calcVec3Checksum(crc, ev->punchangle);
	crc = calcVec3Checksum(crc, ev->v_angle);

	crc = calcVec3Checksum(crc, ev->endpos);
	crc = calcVec3Checksum(crc, ev->startpos);
	crc = calcFloatChecksum(crc, &ev->impacttime);
	crc = calcFloatChecksum(crc, &ev->starttime);

	crc = _mm_crc32_u32(crc, ev->fixangle);
	crc = calcFloatChecksum(crc, &ev->idealpitch);
	crc = calcFloatChecksum(crc, &ev->pitch_speed);
	crc = calcFloatChecksum(crc, &ev->ideal_yaw);
	crc = calcFloatChecksum(crc, &ev->yaw_speed);

	crc = _mm_crc32_u32(crc, ev->modelindex);

	crc = calcEngStringChecksum(crc, ev->model);

	crc = calcEngStringChecksum(crc, ev->viewmodel);
	crc = calcEngStringChecksum(crc, ev->weaponmodel);

	crc = calcVec3Checksum(crc, ev->absmin);
	crc = calcVec3Checksum(crc, ev->absmax);
	crc = calcVec3Checksum(crc, ev->mins);
	crc = calcVec3Checksum(crc, ev->maxs);
	crc = calcVec3Checksum(crc, ev->size);

	crc = calcFloatChecksum(crc, &ev->ltime);
	crc = calcFloatChecksum(crc, &ev->nextthink);

	crc = _mm_crc32_u32(crc, ev->movetype);
	crc = _mm_crc32_u32(crc, ev->solid);

	crc = _mm_crc32_u32(crc, ev->skin);
	crc = _mm_crc32_u32(crc, ev->body);
	crc = _mm_crc32_u32(crc, ev->effects);

	crc = calcFloatChecksum(crc, &ev->gravity);
	crc = calcFloatChecksum(crc, &ev->friction);

	crc = _mm_crc32_u32(crc, ev->light_level);

	crc = _mm_crc32_u32(crc, ev->sequence);
	crc = _mm_crc32_u32(crc, ev->gaitsequence);
	crc = calcFloatChecksum(crc, &ev->frame);
	crc = calcFloatChecksum(crc, &ev->animtime);
	crc = calcFloatChecksum(crc, &ev->framerate);
	crc = _mm_crc32_u32(crc, *(uint32_t*)&ev->controller[0]);
	crc = _mm_crc32_u16(crc, *(uint16_t*)&ev->blending[0]);

	crc = calcFloatChecksum(crc, &ev->scale);

	crc = _mm_crc32_u32(crc, ev->rendermode);
	crc = calcFloatChecksum(crc, &ev->renderamt);
	crc = calcVec3Checksum(crc, ev->rendercolor);
	crc = _mm_crc32_u32(crc, ev->renderfx);

	crc = calcFloatChecksum(crc, &ev->health);
	crc = calcFloatChecksum(crc, &ev->frags);
	crc = _mm_crc32_u32(crc, ev->weapons);
	crc = calcFloatChecksum(crc, &ev->takedamage);

	crc = _mm_crc32_u32(crc, ev->deadflag);
	crc = calcVec3Checksum(crc, ev->view_ofs);

	crc = _mm_crc32_u32(crc, ev->button);
	crc = _mm_crc32_u32(crc, ev->impulse);

	crc = calcEdictRefChecksum(crc, ev->chain);
	crc = calcEdictRefChecksum(crc, ev->dmg_inflictor);
	crc = calcEdictRefChecksum(crc, ev->enemy);
	crc = calcEdictRefChecksum(crc, ev->aiment);
	crc = calcEdictRefChecksum(crc, ev->owner);
	crc = calcEdictRefChecksum(crc, ev->groundentity);
	
	crc = _mm_crc32_u32(crc, ev->spawnflags);
	crc = _mm_crc32_u32(crc, ev->flags);

	crc = _mm_crc32_u32(crc, ev->colormap);
	crc = _mm_crc32_u32(crc, ev->team);

	crc = calcFloatChecksum(crc, &ev->max_health);
	crc = calcFloatChecksum(crc, &ev->teleport_time);
	crc = calcFloatChecksum(crc, &ev->armortype);
	crc = calcFloatChecksum(crc, &ev->armorvalue);

	crc = _mm_crc32_u32(crc, ev->waterlevel);
	crc = _mm_crc32_u32(crc, ev->watertype);

	crc = calcEngStringChecksum(crc, ev->target);
	crc = calcEngStringChecksum(crc, ev->targetname);
	crc = calcEngStringChecksum(crc, ev->netname);
	crc = calcEngStringChecksum(crc, ev->message);

	crc = calcFloatChecksum(crc, &ev->speed);
	crc = calcFloatChecksum(crc, &ev->air_finished);
	crc = calcFloatChecksum(crc, &ev->pain_finished);
	crc = calcFloatChecksum(crc, &ev->radsuit_finished);

	crc = calcEdictRefChecksum(crc, ev->pContainingEntity);

	//int			playerclass;

	crc = calcFloatChecksum(crc, &ev->maxspeed);

	crc = calcFloatChecksum(crc, &ev->fov);
	crc = _mm_crc32_u32(crc, ev->weaponanim);

	crc = _mm_crc32_u32(crc, ev->pushmsec);

	crc = _mm_crc32_u32(crc, ev->bInDuck);
	crc = _mm_crc32_u32(crc, ev->flTimeStepSound);
	crc = _mm_crc32_u32(crc, ev->flSwimTime);
	crc = _mm_crc32_u32(crc, ev->flDuckTime);
	crc = _mm_crc32_u32(crc, ev->iStepLeft);
	crc = calcFloatChecksum(crc, &ev->flFallVelocity);

	crc = _mm_crc32_u32(crc, ev->gamestate);
	crc = _mm_crc32_u32(crc, ev->oldbuttons);
	crc = _mm_crc32_u32(crc, ev->groupinfo);

	return crc;
}

uint32_t calcEdictChecksum(uint32_t crc, const edict_t* ed) {
	crc = _mm_crc32_u32(crc, ed->free);
	if (ed->free)
		return crc;

	crc = _mm_crc32_u32(crc, ed->serialnumber);
	crc = _mm_crc32_u32(crc, ed->headnode);
	crc = _mm_crc32_u32(crc, ed->num_leafs);
	for (int i = 0; i < ed->num_leafs; i++)
		crc = _mm_crc32_u16(crc, ed->leafnums[i]);

	crc = calcFloatChecksum(crc, &ed->freetime);
	crc = calcEntvarsChecksum(crc, &ed->v);
	return crc;
}

void PrintFloat(const float* pVal, std::stringstream &ss) {
	uint32_t sVal = *reinterpret_cast<const uint32_t*>(pVal);
	ss << "{ float: " << *pVal << "; raw: " << std::hex << sVal << " }";
}

void PrintVec3(const vec_t* vec, std::stringstream &ss) {
	ss << "{ vec[0]: "; PrintFloat(&vec[0], ss);
	ss << "; vec[1]: "; PrintFloat(&vec[1], ss);
	ss << "; vec[2]: "; PrintFloat(&vec[2], ss);
	ss << "} ";
	
}

void PrintClipnode(dclipnode_t* clipnode, std::stringstream &ss) {
	ss << "{"
		<< " addr: " << (size_t)clipnode
		<< " planenum: " << clipnode->planenum
		<< " child[0]: " << clipnode->children[0]
		<< " child[1]: " << clipnode->children[1]
		<< " }";
}

void PrintHull(hull_t* hull, std::stringstream &ss)
{
	
	ss << "{"
		<< " addr: " << (size_t)hull
		<< " clip_mins: "; PrintVec3(hull->clip_mins, ss);
	ss << "; clip_maxs: "; PrintVec3(hull->clip_maxs, ss);
	ss << "; firstclipnode: " << hull->firstclipnode
		<< "; lastclipnode: " << hull->lastclipnode
		<< " }";
}

void PrintTrace(trace_t* trace, std::stringstream &ss)
{
	ss << "{ "
		<< " allsolid: " << trace->allsolid
		<< "; startsolid: " << trace->startsolid
		<< "; inopen: " << trace->inopen
		<< "; inwater: " << trace->inwater
		<< "; hitgroup: " << trace->hitgroup
		<< "; ent: ";
	if (trace->ent == NULL)
		ss << "NULL";
	else
		ss << (uint32_t)(trace->ent - g_psv.edicts);

	ss << "; fraction: "; PrintFloat(&trace->fraction, ss);
	ss << "; endpos: "; PrintVec3(trace->endpos, ss);
	ss << "; plane.normal: "; PrintVec3(trace->plane.normal, ss);
	ss << "; plane.dist: "; PrintFloat(&trace->plane.dist, ss);
	ss << " }";
}

enum PrintEdictFlags_e {
	ED_PRINT_POSITION = (1 << 0),
	ED_PRINT_VELOCITY = (1 << 1),
	ED_PRINT_CRC = (1 << 2),
};

void PrintEdict(edict_t* ent, std::stringstream &ss, int flags)
{
	if (ent == NULL) {
		ss << "NULL";
		return;
	}

	ss << "{"
		<< " id: " << (uint32_t)(ent - g_psv.edicts);
	
	if (flags & ED_PRINT_POSITION) {
		ss << "; origin: "; PrintVec3(ent->v.origin, ss);
		ss << "; angles: "; PrintVec3(ent->v.angles, ss);
	}

	if (flags & ED_PRINT_VELOCITY) {
		ss << "; avelocity: "; PrintVec3(ent->v.avelocity, ss);
		ss << "; velocity: "; PrintVec3(ent->v.velocity, ss);
	}

	if (flags & ED_PRINT_CRC) {
		ss << "; crc: " << std::hex << calcEdictChecksum(0, ent);
	}

	ss << " } ";
}

std::ofstream g_RehldsDebugLog;

#ifndef SV_PushRotate_debug_region

SV_PushRotate_proto SV_PushRotate_orig;
int g_SVPushRotateCallQueue = 0;

int __cdecl SV_PushRotate_hooked(edict_t *pusher, float movetime)
{
	static int callCounter = 0;

	std::stringstream ss;
	int currentCallID = ++callCounter;
	ss << " >>> [" << currentCallID << "] SV_PushRotate_hooked( "
		<< "ent: "; PrintEdict(pusher, ss, 0);
	ss << "; movetime: "; PrintFloat(&movetime, ss);
	ss << " )";

	uint32_t entCheckSums[REHLDS_DEBUG_MAX_EDICTS];
	for (int i = 0; i < g_psv.num_edicts; i++)
		entCheckSums[i] = calcEdictChecksum(0, &g_psv.edicts[i]);

	g_RehldsDebugLog << ss.str() << "\n";
	g_RehldsDebugLog.flush();

	g_SVPushRotateCallQueue++;
	int res = SV_PushRotate_orig(pusher, movetime);
	g_SVPushRotateCallQueue--;

	std::stringstream ess;
	ess << " <<< [" << currentCallID << "] SV_PushRotate_hooked( ";
	ess << " ent: "; PrintEdict(pusher, ess, ED_PRINT_CRC);
	ess << " chanedEdicts: [ ";
	
	for (int i = 0; i < g_psv.num_edicts; i++) {
		uint32_t newCrc = calcEdictChecksum(0, &g_psv.edicts[i]);
		if (newCrc != entCheckSums[i]) {
			ess << " { " << i << ": " << entCheckSums[i] << " -> " << newCrc;
			if (65097 == currentCallID) {
				ess << "; ent: "; PrintEdict(&g_psv.edicts[i], ess, ED_PRINT_POSITION | ED_PRINT_VELOCITY);
			}
			ess << " } ";
		}
	}

	ess << " ) => " << res;

	g_RehldsDebugLog << ess.str() << "\n";
	g_RehldsDebugLog.flush();

	return res;
}
#endif


#ifndef SV_RecursiveHullCheck_debug_region

SV_RecursiveHullCheck_proto SV_RecursiveHullCheck_orig;

qboolean __cdecl SV_RecursiveHullCheck_hooked(hull_t *hull, int num, float p1f, float p2f, vec_t *p1, vec_t *p2, trace_t *trace)
{
	static int callCounter = 0;

	std::stringstream ss;
	int currentCallID = ++callCounter;
	ss << " >>> [" << currentCallID << "] SV_RecursiveHullCheck_hooked( "
		<< "hull: "; PrintHull(hull, ss);
	ss << "; num: " << num << "; p1f: "; PrintFloat(&p1f, ss);
	ss << "; p2f: "; PrintFloat(&p2f, ss);
	ss << "; p1: "; PrintVec3(p1, ss);
	ss << "; p2: "; PrintVec3(p2, ss);
	ss << "; trace: "; PrintTrace(trace, ss);
	ss << ")";

	g_RehldsDebugLog << ss.str() << "\n";
	g_RehldsDebugLog.flush();

	qboolean res = SV_RecursiveHullCheck_orig(hull, num, p1f, p2f, p1, p2, trace);

	std::stringstream ess;
	ess << " <<< [" << currentCallID << "] SV_RecursiveHullCheck_hooked( "
		<< "trace: "; PrintTrace(trace, ess);
	ess << " ) => " << res;

	g_RehldsDebugLog << ess.str() << "\n";
	g_RehldsDebugLog.flush();

	return res;
}

#endif

#ifndef SV_SingleClipMoveToEntity_debug_region

SV_SingleClipMoveToEntity_proto SV_SingleClipMoveToEntity_orig;

void __cdecl SV_SingleClipMoveToEntity_hooked(edict_t *ent, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, trace_t *trace)
{
	static int callCounter = 0;

	std::stringstream ss;
	int currentCallID = ++callCounter;
	ss << " >>> [" << currentCallID << "] SV_SingleClipMoveToEntity_hooked( "
		<< "ent: ";

	if (ent == NULL)
		ss << "NULL";
	else
		ss << (uint32_t)(ent - g_psv.edicts);

	ss << "; start: "; PrintVec3(start, ss);
	ss << "; mins: "; PrintVec3(mins, ss);
	ss << "; maxs: "; PrintVec3(maxs, ss);
	ss << "; end: "; PrintVec3(end, ss);
	ss << ")";

	g_RehldsDebugLog << ss.str() << "\n";
	g_RehldsDebugLog.flush();

	SV_SingleClipMoveToEntity_orig(ent, start, mins, maxs, end, trace);

	std::stringstream ess;
	ess << " <<< [" << currentCallID << "] SV_SingleClipMoveToEntity_hooked( "
		<< "trace: "; PrintTrace(trace, ess);
	ess << " )";

	g_RehldsDebugLog << ess.str() << "\n";
	g_RehldsDebugLog.flush();
}

#endif

#ifndef SV_PushEntity_debug_region
SV_PushEntity_proto SV_PushEntity_orig;

trace_t __cdecl SV_PushEntity_hooked(edict_t *ent, vec_t *push)
{
	static int callCounter = 0;

	if (g_SVPushRotateCallQueue <= 0 || true) {
		return SV_PushEntity_orig(ent, push);
	}

	std::stringstream ss;
	int currentCallID = ++callCounter;
	ss << " >>> [" << currentCallID << "] SV_PushEntity_hooked( "
		<< "ent: "; PrintEdict(ent, ss, 0);
	ss << "; push: "; PrintVec3(push, ss);
	ss << ")";

	g_RehldsDebugLog << ss.str() << "\n";
	g_RehldsDebugLog.flush();

	trace_t res = SV_PushEntity_orig(ent, push);

	std::stringstream ess;
	ess << " <<< [" << currentCallID << "] SV_PushEntity_hooked( ";
	ess << " ) ";

	g_RehldsDebugLog << ess.str() << "\n";
	g_RehldsDebugLog.flush();

	return res;
}
#endif

#ifndef SV_LinkEdict_debug_region

SV_LinkEdict_proto SV_LinkEdict_orig;

void __cdecl SV_LinkEdict_hooked(edict_t *ent, qboolean touch_triggers)
{
	static int callCounter = 0;

	if (g_SVPushRotateCallQueue <= 0) {
		SV_LinkEdict_orig(ent, touch_triggers);
		return;
	}

	std::stringstream ss;
	int currentCallID = ++callCounter;
	ss << " >>> [" << currentCallID << "] SV_LinkEdict_hooked( "
		<< "ent: "; PrintEdict(ent, ss, ED_PRINT_CRC);
	ss << "; touch_triggers: " << touch_triggers
		<< " )";

	g_RehldsDebugLog << ss.str() << "\n";
	g_RehldsDebugLog.flush();

	SV_LinkEdict_orig(ent, touch_triggers);

	std::stringstream ess;
	ess << " <<< [" << currentCallID << "] SV_LinkEdict_hooked( ";
	ess << " ) ";

	g_RehldsDebugLog << ess.str() << "\n";
	g_RehldsDebugLog.flush();
}

#endif

#ifndef ran1_debug_region

ran1_proto ran1_orig;
int32 __cdecl ran1_hooked()
{
	static int callCounter = 0;

	std::stringstream ss;
	int currentCallID = ++callCounter;
	ss << " >>> [" << currentCallID << "] ran1() ";
	g_RehldsDebugLog << ss.str() << "\n";
	g_RehldsDebugLog.flush();

	int res = ran1_orig();

	std::stringstream ess;
	ess << " <<< [" << currentCallID << "] ran1() => " << res;

	g_RehldsDebugLog << ess.str() << "\n";
	g_RehldsDebugLog.flush();

	return res;
}

#endif

#ifndef SV_Physics_Step_debug_region

SV_Physics_Step_proto SV_Physics_Step_orig;

void __cdecl SV_Physics_Step_hooked(edict_t *ent)
{
	static int callCounter = 0;

	std::stringstream ss;
	int currentCallID = ++callCounter;
	ss << " >>> [" << currentCallID << "] SV_Physics_Step_hooked( "
		<< "ent: "; PrintEdict(ent, ss, ED_PRINT_CRC | ED_PRINT_VELOCITY | ED_PRINT_POSITION);
	ss << ")";

	g_RehldsDebugLog << ss.str() << "\n";
	g_RehldsDebugLog.flush();

	SV_Physics_Step_orig(ent);

	std::stringstream ess;
	ess << " <<< [" << currentCallID << "] SV_Physics_Step_hooked( "
		<< "ent: "; PrintEdict(ent, ess, ED_PRINT_CRC | ED_PRINT_VELOCITY | ED_PRINT_POSITION);
	ess << " ) ";

	g_RehldsDebugLog << ess.str() << "\n";
	g_RehldsDebugLog.flush();
}

#endif

void Rehlds_Debug_logAlloc(size_t sz, void* ptr)
{
	g_RehldsDebugLog << "malloc(" << sz << ") => " << std::hex << (size_t)ptr << "\n";
	g_RehldsDebugLog.flush();
}

void Rehlds_Debug_logRealloc(size_t sz, void* oldPtr, void* newPtr)
{
	g_RehldsDebugLog << "realloc(" << std::hex << (size_t)oldPtr << ", " << sz << ") => " << std::hex << (size_t)newPtr << "\n";
	g_RehldsDebugLog.flush();
}

void Rehlds_Debug_logFree(void* ptr)
{
	g_RehldsDebugLog << "free(" << std::hex << (size_t)ptr << ")\n";
	g_RehldsDebugLog.flush();
}


void Rehlds_Debug_Init(Module* engine)
{
	//CFuncAddr* cfa;

	/*
	CFuncAddr* recursiveHullCheckCalls = NULL;
	FindAllCalls(&engine->codeSection, &recursiveHullCheckCalls, 0xBA160 + engine->base);
	SV_RecursiveHullCheck_orig = (SV_RecursiveHullCheck_proto)((void*)(0xBA160 + engine->base));

	cfa = recursiveHullCheckCalls;
	while (cfa != NULL) {
		HookFunctionCall((void*)cfa->Addr, (void*)&SV_RecursiveHullCheck_hooked);
		cfa = cfa->Next;
	}
	

	CFuncAddr* singleClipMoveToEntityCalls = NULL;
	FindAllCalls(&engine->codeSection, &singleClipMoveToEntityCalls, 0xBA550 + engine->base);
	SV_SingleClipMoveToEntity_orig = (SV_SingleClipMoveToEntity_proto)((void*)(0xBA550 + engine->base));

	cfa = singleClipMoveToEntityCalls;
	while (cfa != NULL) {
		HookFunctionCall((void*)cfa->Addr, (void*)&SV_SingleClipMoveToEntity_hooked);
		cfa = cfa->Next;
	}

	*/

	/*
	CFuncAddr* pushEntityCalls = NULL;
	FindAllCalls(&engine->codeSection, &pushEntityCalls, 0x95450 + engine->base);
	SV_PushEntity_orig = (SV_PushEntity_proto)((void*)(0x95450 + engine->base));

	cfa = pushEntityCalls;
	while (cfa != NULL) {
		HookFunctionCall((void*)cfa->Addr, (void*)&SV_PushEntity_hooked);
		cfa = cfa->Next;
	}
	*/


	/*
	CFuncAddr* linkEdictCalls = NULL;
	FindAllCalls(&engine->codeSection, &linkEdictCalls, 0xB9CF0 + engine->base);
	SV_LinkEdict_orig = (SV_LinkEdict_proto)((void*)(0xB9CF0 + engine->base));

	cfa = linkEdictCalls;
	while (cfa != NULL) {
		HookFunctionCall((void*)cfa->Addr, (void*)&SV_LinkEdict_hooked);
		cfa = cfa->Next;
	}
	*/

	/*
	CFuncAddr* pushRotateCalls = NULL;
	FindAllCalls(&engine->codeSection, &pushRotateCalls, 0x958F0 + engine->base);
	SV_PushRotate_orig = (SV_PushRotate_proto)((void*)(0x958F0 + engine->base));

	cfa = pushRotateCalls;
	while (cfa != NULL) {
		HookFunctionCall((void*)cfa->Addr, (void*)&SV_PushRotate_hooked);
		cfa = cfa->Next;
	}
	*/

	/*
	CFuncAddr* ran1Calls = NULL;
	FindAllCalls(&engine->codeSection, &ran1Calls, 0x60A10 + engine->base);
	ran1_orig = (ran1_proto)((void*)(0x60A10 + engine->base));

	cfa = ran1Calls;
	while (cfa != NULL) {
		HookFunctionCall((void*)cfa->Addr, (void*)&ran1_hooked);
		cfa = cfa->Next;
	}
	*/

	/*
	CFuncAddr* svPysicsStepCallc = NULL;
	FindAllCalls(&engine->codeSection, &svPysicsStepCallc, 0x96E70 + engine->base);
	SV_Physics_Step_orig = (SV_Physics_Step_proto)((void*)(0x96E70 + engine->base));

	cfa = svPysicsStepCallc;
	while (cfa != NULL) {
		HookFunctionCall((void*)cfa->Addr, (void*)&SV_Physics_Step_hooked);
		cfa = cfa->Next;
	}
	*/

	
	//g_RehldsDebugLog.exceptions(std::ios::badbit | std::ios::failbit);
	//g_RehldsDebugLog.open("d:\\rehlds_debug.log", std::ios::out | std::ios::binary);


}
