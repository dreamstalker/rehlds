#ifndef ICOMMANDLINE_H
#define ICOMMANDLINE_H
#ifdef _WIN32
#pragma once
#endif

// Interface to engine command line
class ICommandLine {
public:
	virtual void CreateCmdLine(const char *commandline) = 0;
	virtual void CreateCmdLine(int argc, const char **argv) = 0;
	virtual const char *GetCmdLine() const = 0;

	// Check whether a particular parameter exists
	virtual	const char *CheckParm(const char *psz, char **ppszValue = nullptr) const = 0;
	virtual void RemoveParm(const char *pszParm) = 0;
	virtual void AppendParm(const char *pszParm, const char *pszValues) = 0;

	virtual void SetParm(const char *pszParm, const char *pszValues) = 0;
	virtual void SetParm(const char *pszParm, int iValue) = 0;
};

ICommandLine *CommandLine();

#endif // ICOMMANDLINE_H
