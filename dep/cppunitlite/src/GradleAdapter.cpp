#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cppunitlite/GradleAdapter.h"
#include "cppunitlite/Test.h"
#include "cppunitlite/TestRegistry.h"

int GradleAdapter::writeAllTestsInfoToFile(const char* fname) {
	FILE* outFile = fopen(fname, "w");
	if (outFile == NULL) {
		return 1;
	}

	fprintf(outFile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	fprintf(outFile, "<tests>\n");

	Test* curTest = TestRegistry::getFirstTest();
	while (curTest != NULL) {
		fprintf(outFile, "<test ");

		fprintf(outFile, " name=\"%s\" ", curTest->getName());
		fprintf(outFile, " group=\"%s\" ", curTest->getGroup());
		fprintf(outFile, " timeout=\"%d\" ", curTest->getTimeout());

		fprintf(outFile, "/>\n");
		curTest = curTest->getNext();
	}

	fprintf(outFile, "</tests>\n");
	fclose(outFile);
	return 0;
}

int GradleAdapter::runTest(const char* groupName, const char* testName) {
	Test* curTest = TestRegistry::getFirstTest();
	while (curTest != NULL) {
		if (!strcmp(groupName, curTest->getGroup()) && !strcmp(testName, curTest->getName())) {
			break;
		}
		curTest = curTest->getNext();
	}

	if (curTest == NULL) {
		printf("Test group='%s' name='%s' not found\n", groupName, testName);
		return 2;
	}

	TestResult result;
	curTest->run(result);

	if (result.getFailureCount()) {
		return 1;
	}
	else {
		return 0;
	}
}

int GradleAdapter::runGroup(const char* groupName) {
	Test* curTest = TestRegistry::getFirstTest();
	int ranTests = 0;
	while (curTest != NULL) {
		if (strcmp(groupName, curTest->getGroup())) {
			curTest = curTest->getNext();
			continue;
		}

		TestResult result;
		curTest->run(result);
		ranTests++;

		if (result.getFailureCount()) {
			return 1;
		}

		curTest = curTest->getNext();
	}

	if (ranTests == 0) {
		printf("No tests with group='%s' found\n", groupName);
		return 2;
	}

	printf("There were no test failures; Tests executed: %d\n", ranTests);
	return 0;
}

int GradleAdapter::runAllTests() {
	Test* curTest = TestRegistry::getFirstTest();
	int ranTests = 0;
	while (curTest != NULL) {
		TestResult result;
		curTest->run(result);
		ranTests++;

		if (result.getFailureCount()) {
			return 1;
		}

		curTest = curTest->getNext();
	}

	printf("There were no test failures; Tests executed: %d\n", ranTests);
	return 0;
}

int GradleAdapter::testsEntryPoint(int argc, char* argv[]) {
	if (argc < 2 || !strcmp(argv[1], "-all")) {
		return runAllTests();
	}

	if (!strcmp(argv[1], "-writeTestInfo")) {
		if (argc != 3) {
			printf("-writeTestInfo requires file name\n");
		}

		return writeAllTestsInfoToFile(argv[2]);
	}

	if (!strcmp(argv[1], "-runTest")) {
		if (argc != 4) {
			printf("-runTest requires group name and test name\n");
		}

		return runTest(argv[2], argv[3]);
	}

	if (!strcmp(argv[1], "-runGroup")) {
		if (argc != 3) {
			printf("-runGroup requires group name\n");
		}

		return runGroup(argv[2]);
	}

	printf("Bad argument specified\n");
	return 1;
}
