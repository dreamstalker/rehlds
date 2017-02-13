#include "precompiled.h"

class CPerformanceCounter
{
public:
	CPerformanceCounter();

	void InitializePerformanceCounter();
	double GetCurTime();

private:
	int m_iLowShift;
	double m_flPerfCounterFreq;
	double m_flCurrentTime;
	double m_flLastCurrentTime;
};

inline CPerformanceCounter::CPerformanceCounter() :
	m_iLowShift(0),
	m_flPerfCounterFreq(0),
	m_flCurrentTime(0),
	m_flLastCurrentTime(0)
{
	InitializePerformanceCounter();
}

inline void CPerformanceCounter::InitializePerformanceCounter()
{
#ifdef _WIN32

	LARGE_INTEGER performanceFreq;
	QueryPerformanceFrequency(&performanceFreq);

	// get 32 out of the 64 time bits such that we have around
	// 1 microsecond resolution
	unsigned int lowpart, highpart;
	lowpart = (unsigned int)performanceFreq.LowPart;
	highpart = (unsigned int)performanceFreq.HighPart;
	m_iLowShift = 0;

	while (highpart || (lowpart > 2000000.0))
	{
		m_iLowShift++;
		lowpart >>= 1;
		lowpart |= (highpart & 1) << 31;
		highpart >>= 1;
	}

	m_flPerfCounterFreq = 1.0 / (double)lowpart;

#endif // _WIN32
}

inline double CPerformanceCounter::GetCurTime()
{
#ifdef _WIN32

	static int sametimecount;
	static unsigned int oldtime;
	static int first = 1;
	LARGE_INTEGER PerformanceCount;
	unsigned int temp, t2;
	double time;

	QueryPerformanceCounter(&PerformanceCount);
	if (m_iLowShift == 0)
	{
		temp = (unsigned int)PerformanceCount.LowPart;
	}
	else
	{
		temp = ((unsigned int)PerformanceCount.LowPart >> m_iLowShift) |
			((unsigned int)PerformanceCount.HighPart << (32 - m_iLowShift));
	}

	if (first)
	{
		oldtime = temp;
		first = 0;
	}
	else
	{
		// check for turnover or backward time
		if ((temp <= oldtime) && ((oldtime - temp) < 0x10000000))
		{
			// so we can't get stuck
			oldtime = temp;
		}
		else
		{
			t2 = temp - oldtime;

			time = (double)t2 * m_flPerfCounterFreq;
			oldtime = temp;

			m_flCurrentTime += time;

			if (m_flCurrentTime == m_flLastCurrentTime)
			{
				if (++sametimecount > 100000)
				{
					m_flCurrentTime += 1.0;
					sametimecount = 0;
				}
			}
			else
			{
				sametimecount = 0;
			}

			m_flLastCurrentTime = m_flCurrentTime;
		}
	}

	return m_flCurrentTime;

#else // _WIN32

	struct timeval tp;
	static int secbase = 0;

	gettimeofday(&tp, NULL);

	if (!secbase)
	{
		secbase = tp.tv_sec;
		return (tp.tv_usec / 1000000.0);
	}

	return ((tp.tv_sec - secbase) + tp.tv_usec / 1000000.0);

#endif // _WIN32
}

void _LogFunctionTrace(const char *pFunctionName, const char *param)
{
	//{
	//	char *entry;
	//}
}

double _StartFunctionTimer()
{
	//CPerformanceCounter::GetCurTime();
	return 0;
}

void _LogFunctionTraceMaxTime(const char *pFunctionName, double startTime, double maxTime)
{
	//{
	//	double timeDiff;
	//	CPerformanceCounter::GetCurTime();
	//	_LogFunctionTrace(const char *pFunctionName, const char *param);
	//}
}

void ClearErrorLogs()
{
}

void Error(const char *pMsg, ...)
{
	//{
	//	char logName;
	//	FILE *f;
	//	va_list args;
	//	{
	//		int i;
	//	}
	//}
}
