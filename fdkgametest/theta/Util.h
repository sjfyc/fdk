#ifndef __UTIL_H_INCLUDE__
#define __UTIL_H_INCLUDE__
#include "Types.h"
#include <iostream>
#include <time.h>

namespace util
{
	void output(const char* fmt, ...);
	
	void fillCell(const CellCoord& coord, DWORD color);
	void drawVertex(const VertexCoord& vertex, DWORD color);

	CellCoord locationToCellCoord(const Location& location);
	Location cellCoordToLocation(const CellCoord& cellCoord);

	CellRange locationRangeToCellRange(const LocationRange& locationRange);
	LocationRange cellRangeToLocationRange(const CellRange& cellRange);

	VertexCoord locationToVertexCoord(const Location& location);
	VertexCoord locationToNearestVertexCoord(const Location& location);
	Location vertexCoordToLocation(const VertexCoord& vertexCoord);

	double getSeconds();
}

#endif
