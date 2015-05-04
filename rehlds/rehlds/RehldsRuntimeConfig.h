#pragma once

enum TestPlayerMode {
	TPM_DISABLE,
	TPM_RECORD,
	TPM_PLAY,
	TPM_ANONYMIZE,
};

class CRehldsRuntimeConfig {
private:
	static const char* getNextToken(char** pBuf);

public:
	CRehldsRuntimeConfig();

	bool disableAllHooks;
	char testRecordingFileName[260];
	TestPlayerMode testPlayerMode;

	void parseFromCommandLine(const char* cmdLine);
};

extern CRehldsRuntimeConfig g_RehldsRuntimeConfig;
