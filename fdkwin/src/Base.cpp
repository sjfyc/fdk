#include <fdkwin/Encode.h>

namespace fdk { namespace win 
{
	FDKWIN_API double getSecondsPerCycle()
	{
		LARGE_INTEGER frequency;
		BOOL result = QueryPerformanceFrequency(&frequency);
		assert(result);
		return 1.0 / frequency.QuadPart;
	}
}}
