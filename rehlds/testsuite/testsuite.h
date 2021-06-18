#pragma once
#ifdef _WIN32

#include "osconfig.h"
#include "memory.h"
#include "rehlds/platform.h"

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

const int TESTSUITE_PROTOCOL_VERSION_MINOR = 5;
const int TESTSUITE_PROTOCOL_VERSION_MAJOR = 0;

void TestSuite_Init(const Module* engine, const Module* executable, const AddressRef* funcRefs);

#endif
