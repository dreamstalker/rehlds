#include "precompiled.h"
#include "rehlds_tests_shared.h"
#include "cppunitlite/GradleAdapter.h"

int main(int argc, char* argv[]) {
	printf("TestRunner: main()\n");

	GradleAdapter a;
	return a.testsEntryPoint(argc, argv);
}
