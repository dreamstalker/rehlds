#include "precompiled.h"

TokenLine::TokenLine()
{
	memset(m_token, 0, sizeof(m_token));
	memset(m_fullLine, 0, sizeof(m_fullLine));
	memset(m_tokenBuffer, 0, sizeof(m_tokenBuffer));

	m_tokenNumber = 0;
}

TokenLine::TokenLine(char *string)
{
	SetLine(string);
}

TokenLine::~TokenLine()
{

}

bool TokenLine::SetLine(const char *newLine)
{
	m_tokenNumber = 0;

	if (!newLine || (strlen(newLine) >= (MAX_LINE_CHARS - 1)))
	{
		memset(m_fullLine, 0, sizeof(m_fullLine));
		memset(m_tokenBuffer, 0, sizeof(m_tokenBuffer));
		return false;
	}

	strcopy(m_fullLine, newLine);
	strcopy(m_tokenBuffer, newLine);

	// parse tokens
	char *charPointer = m_tokenBuffer;
	while (*charPointer && (m_tokenNumber < MAX_LINE_TOKENS))
	{
		// skip nonprintable chars
		while (*charPointer && ((*charPointer <= ' ') || (*charPointer > '~')))
			charPointer++;

		if (*charPointer)
		{
			m_token[m_tokenNumber] = charPointer;

			// special treatment for quotes
			if (*charPointer == '\"')
			{
				charPointer++;
				m_token[m_tokenNumber] = charPointer;
				while (*charPointer && (*charPointer != '\"'))
					charPointer++;
			}
			else
			{
				m_token[m_tokenNumber] = charPointer;
				while (*charPointer && ((*charPointer > 32) && (*charPointer <= 126)))
					charPointer++;
			}

			m_tokenNumber++;

			if (*charPointer)
			{
				*charPointer = '\0';
				charPointer++;
			}
		}
	}

	return (m_tokenNumber != MAX_LINE_TOKENS);
}

char *TokenLine::GetLine()
{
	return m_fullLine;
}

char *TokenLine::GetToken(int i)
{
	if (i >= m_tokenNumber)
		return NULL;

	return m_token[i];
}

// if the given parm is not present return NULL
// otherwise return the address of the following token, or an empty string
char *TokenLine::CheckToken(char *parm)
{
	for (int i = 0; i < m_tokenNumber; i++)
	{
		if (!m_token[i])
			continue;

		if (!strcmp(parm, m_token[i]))
		{
			char *ret = m_token[i + 1];

			// if this token doesn't exist, since index i was the last
			// return an empty string
			if (m_tokenNumber == (i + 1))
				ret = "";

			return ret;
		}
	}

	return NULL;
}

int TokenLine::CountToken()
{
	int c = 0;
	for (int i = 0; i < m_tokenNumber; i++)
	{
		if (m_token[i])
			c++;
	}

	return c;
}

char *TokenLine::GetRestOfLine(int i)
{
	if (i >= m_tokenNumber)
		return NULL;

	return m_fullLine + (m_token[i] - m_tokenBuffer);
}
