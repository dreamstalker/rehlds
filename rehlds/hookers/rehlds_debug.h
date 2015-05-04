#pragma once

#include "osconfig.h"
#include "maintypes.h"
#include "memory.h"
#include "model.h"


#ifdef _WIN32

typedef qboolean(__cdecl *SV_RecursiveHullCheck_proto)(hull_t *hull, int num, float p1f, float p2f, vec_t *p1, vec_t *p2, trace_t *trace);
extern SV_RecursiveHullCheck_proto SV_RecursiveHullCheck_orig;
extern qboolean __cdecl SV_RecursiveHullCheck_hooked(hull_t *hull, int num, float p1f, float p2f, vec_t *p1, vec_t *p2, trace_t *trace);

typedef void(__cdecl *SV_SingleClipMoveToEntity_proto)(edict_t *ent, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, trace_t *trace);
extern SV_SingleClipMoveToEntity_proto SV_SingleClipMoveToEntity_orig;
void __cdecl SV_SingleClipMoveToEntity_hooked(edict_t *ent, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, trace_t *trace);

typedef trace_t (__cdecl *SV_PushEntity_proto)(edict_t *ent, vec_t *push);
extern SV_PushEntity_proto SV_PushEntity_orig;
trace_t __cdecl SV_PushEntity_hooked(edict_t *ent, vec_t *push);

typedef void (__cdecl *SV_LinkEdict_proto)(edict_t *ent, qboolean touch_triggers);
extern SV_LinkEdict_proto SV_LinkEdict_orig;
void __cdecl SV_LinkEdict_hooked(edict_t *ent, qboolean touch_triggers);

typedef int(__cdecl *SV_PushRotate_proto)(edict_t *pusher, float movetime);
extern SV_PushRotate_proto SV_PushRotate_orig;
int __cdecl SV_PushRotate_hooked(edict_t *pusher, float movetime);

typedef int32(__cdecl *ran1_proto)();
extern ran1_proto ran1_orig;
int32 __cdecl ran1_hooked();

typedef void (__cdecl *SV_Physics_Step_proto)(edict_t *ent);
extern SV_Physics_Step_proto SV_Physics_Step_orig;
void __cdecl SV_Physics_Step_hooked(edict_t *ent);


extern void Rehlds_Debug_logAlloc(size_t sz, void* ptr);
extern void Rehlds_Debug_logRealloc(size_t sz, void* oldPtr, void* newPtr);
extern void Rehlds_Debug_logFree(void* ptr);

extern void Rehlds_Debug_Init(Module* engine);

#endif
