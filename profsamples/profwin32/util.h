#include <Windows.h>

namespace util
{
	inline double getSecondsPerCycle()
	{
		LARGE_INTEGER frequency;
		BOOL result = QueryPerformanceFrequency(&frequency);
		return 1.0 / frequency.QuadPart;
	}

	inline double getSeconds()
	{
		static double secondsPerCycle = getSecondsPerCycle();
		LARGE_INTEGER cycles;
		QueryPerformanceCounter(&cycles);
		// Add big number to make bugs apparent where return value is being passed to FLOAT
		return cycles.QuadPart * secondsPerCycle + 16777216.0;
	}

	struct ScopeProfileRecorder
	{
		ScopeProfileRecorder()
		{
			start = getSeconds();
		}
		~ScopeProfileRecorder()
		{
			std::cout << getSeconds() - start << std::endl;
		}
		double start;
	};
#define UTIL_SCOPE_PROF	::util::ScopeProfileRecorder __scopeProfileRecorder;
}
