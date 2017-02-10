#include "precompiled.h"

CRehldsRuntimeConfig g_RehldsRuntimeConfig;

CRehldsRuntimeConfig::CRehldsRuntimeConfig()
{
	disableAllHooks = false;
	testRecordingFileName[0] = 0;
	testPlayerMode = TPM_DISABLE;
}

void CRehldsRuntimeConfig::parseFromCommandLine(const char* cmdLine) {
	char localBuf[2048];
	if (strlen(cmdLine) >= sizeof(localBuf)) rehlds_syserror("%s: too long cmdline", __func__);

	strcpy(localBuf, cmdLine);
	char* cpos = localBuf;

	getNextToken(&cpos); //skip executable path

	const char* token = getNextToken(&cpos);
	while (token != NULL) {
		if (!strcmp(token, "--rehlds-test-record"))
		{
			const char* fname = getNextToken(&cpos);
			if (fname == NULL) rehlds_syserror("%s: usage: --rehlds-test-record <filename>", __func__);
			strncpy(testRecordingFileName, fname, sizeof(testRecordingFileName));
			testRecordingFileName[sizeof(testRecordingFileName) - 1] = 0;
			testPlayerMode = TPM_RECORD;
		}
		else if (!strcmp(token, "--rehlds-test-play"))
		{
			const char* fname = getNextToken(&cpos);
			if (fname == NULL) rehlds_syserror("%s: usage: --rehlds-test-play <filename>", __func__);
			strncpy(testRecordingFileName, fname, sizeof(testRecordingFileName));
			testRecordingFileName[sizeof(testRecordingFileName) - 1] = 0;
			testPlayerMode = TPM_PLAY;
		}
		else if (!strcmp(token, "--rehlds-test-anon"))
		{
			const char* fname = getNextToken(&cpos);
			if (fname == NULL) rehlds_syserror("%s: usage: --rehlds-test-anon <filename>", __func__);
			strncpy(testRecordingFileName, fname, sizeof(testRecordingFileName));
			testRecordingFileName[sizeof(testRecordingFileName) - 1] = 0;
			testPlayerMode = TPM_ANONYMIZE;
		}
		else if (!strcmp(token, "--rehlds-disable-all-hooks"))
		{
			disableAllHooks = true;
		}

		token = getNextToken(&cpos);
	}
}

const char* CRehldsRuntimeConfig::getNextToken(char** pBuf) {
	char* rpos = *pBuf;
	if (*rpos == 0) return NULL;

	//skip spaces at the beginning
	while (*rpos != 0 && isspace(*rpos))
		rpos++;

	if (*rpos == 0) {
		*pBuf = rpos;
		return NULL;
	}

	const char* res = rpos;
	char* wpos = rpos;
	char inQuote = 0;
	while (*rpos != 0) {
		char cc = *rpos;
		if (inQuote) {
			if (inQuote == cc)
			{
				inQuote = 0;
				rpos++;
			}
			else
			{
				if (rpos != wpos) *wpos = cc;
				rpos++; wpos++;
			}
		}
		else
		{
			if (isspace(cc))
			{
				break;
			}
			else if (cc == '\'' || cc == '"')
			{
				inQuote = cc;
				rpos++;
			}
			else
			{
				if (rpos != wpos) *wpos = cc;
				rpos++; wpos++;
			}
		}
	}

	if (*rpos != 0) {
		rpos++;
	}
	*pBuf = rpos;

	*wpos = 0;

	return res;
}
