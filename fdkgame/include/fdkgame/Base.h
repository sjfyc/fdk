#ifndef __FDKGAME_BASE_H_INCLUDE__
#define __FDKGAME_BASE_H_INCLUDE__

/*
Build dll: Define FDKGAME_DLL FDKGAME_EXPORTS
Use dll: Define FDKGAME_DLL
Build/Use StaticLib: Nothing need to be defined
*/
#ifdef FDKGAME_DLL
	#ifdef FDKGAME_EXPORTS
	#define FDKGAME_API __declspec(dllexport)
	#else
	#define FDKGAME_API __declspec(dllimport)
	#endif
#else
	#define FDKGAME_API
#endif

#include <fdk/Base.h>
#include <utility> // std::swap

namespace fdk { namespace game {} }
namespace fdkgame=fdk::game;

namespace fdk { namespace game 
{	
}}

#endif
