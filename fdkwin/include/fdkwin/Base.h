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

namespace fdk { namespace win {} }
namespace fdkwin=fdk::win;

#endif
