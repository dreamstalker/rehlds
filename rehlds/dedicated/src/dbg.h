#ifndef DBG_H
#define DBG_H
#ifdef _WIN32
#pragma once
#endif

void _LogFunctionTrace(const char *pFunctionName, const char *param);
double _StartFunctionTimer();
void _LogFunctionTraceMaxTime(const char *pFunctionName, double startTime, double maxTime);
void ClearErrorLogs();
void Error(const char *pMsg, ...);

#endif // DBG_H
