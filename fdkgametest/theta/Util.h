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

	CellRange locationRangeToCellRange(const LocationRange& locationRange);
	LocationRange cellRangeToLocationRange(const CellRange& cellRange);

	VertexCoord locationToVertexCoord(const Location& location);
	VertexCoord locationToVertexCoord(const Location& location, bool& xAlign, bool& yAlign);
	Location vertexCoordToLocation(const VertexCoord& vertexCoord);
}

#endif
