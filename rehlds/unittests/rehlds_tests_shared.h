#pragma once

#include "osconfig.h"
#include "engine.h"

extern void Tests_InitEngine();
extern void Tests_ShutdownEngine();

class EngineInitializer {
public:
	EngineInitializer() {
		Tests_InitEngine();
	}

	~EngineInitializer() {
		Tests_ShutdownEngine();
	}
};