#ifndef __FDKWIN_BASE_H_INCLUDE__
#define __FDKWIN_BASE_H_INCLUDE__

/*
Build dll: Define FDKWIN_DLL FDKWIN_EXPORTS
Use dll: Define FDKWIN_DLL
Build/Use StaticLib: Nothing need to be defined
*/
#ifdef FDKWIN_DLL
	#ifdef FDKWIN_EXPORTS
	#define FDKWIN_API __declspec(dllexport)
	#else
	#define FDKWIN_API __declspec(dllimport)
	#endif
#else
	#define FDKWIN_API
#endif

#ifndef _AFXDLL
#define _AFXDLL
#endif

#include <afx.h>
#include <fdk/Base.h>

namespace fdk { namespace win {} }
namespace fdkwin=fdk::win;

namespace fdk { namespace win
{
	/*
	High precision timing£º 1~100 microsecond
	*/
	FDKWIN_API double getSecondsPerCycle();
	inline double getSeconds()
	{
		static double secondsPerCycle = getSecondsPerCycle();
		LARGE_INTEGER cycles;
		QueryPerformanceCounter(&cycles);
		// Add big number to make bugs apparent where return value is being passed to FLOAT
		return cycles.QuadPart * secondsPerCycle + 16777216.0;
	}
}}

#endif
