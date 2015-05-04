#include "precompiled.h"
#include "cppunitlite/GradleAdapter.h"

int main(int argc, char* argv[]) {
	printf("TestRunner: main()\n");

	GradleAdapter a;
	int res = a.testsEntryPoint(argc, argv);

#ifdef _BUILD_FROM_IDE
	system("PAUSE");
#endif

	return res;
}
