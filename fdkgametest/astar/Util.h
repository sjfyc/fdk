#ifndef __UTIL_H_INCLUDE__
#define __UTIL_H_INCLUDE__
#include "Types.h"
#include <iostream>

namespace util
{
	void output(const char* fmt, ...);
	void fillCell(const CellCoord& coord, DWORD color);
	CellCoord locationToCellCoord(const Location& location);
	Location cellCoordToLocation(const CellCoord& cellCoord);
}

#endif
