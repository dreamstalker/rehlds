#pragma once

class GradleAdapter {
public:
	int writeAllTestsInfoToFile(const char* fname);
	int runTest(const char* groupName, const char* testName);
	int runAllTests();
	int testsEntryPoint(int argc, char* argv[]);
};
