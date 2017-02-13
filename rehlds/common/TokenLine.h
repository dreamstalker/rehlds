#ifndef TOKENLINE_H
#define TOKENLINE_H
#ifdef _WIN32
#pragma once
#endif

class TokenLine {
public:
	TokenLine();
	TokenLine(char *string);
	virtual ~TokenLine();

	char *GetRestOfLine(int i);		// returns all chars after token i
	int CountToken();			// returns number of token
	char *CheckToken(char *parm);		// returns token after token parm or ""
	char *GetToken(int i);			// returns token i
	char *GetLine();			// returns full line
	bool SetLine(const char  *newLine);	// set new token line and parses it

private:
	enum { MAX_LINE_CHARS = 2048, MAX_LINE_TOKENS = 128 };

	char m_tokenBuffer[MAX_LINE_CHARS];
	char m_fullLine[MAX_LINE_CHARS];
	char *m_token[MAX_LINE_TOKENS];
	int m_tokenNumber;
};

#endif // TOKENLINE_H
