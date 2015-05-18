#pragma once

class GradleAdapter {
public:
	int writeAllTestsInfoToFile(const char* fname);
	int runTest(const char* groupName, const char* testName);
	int runGroup(const char* groupName);
	int runAllTests();
	int testsEntryPoint(int argc, char* argv[]);
};
