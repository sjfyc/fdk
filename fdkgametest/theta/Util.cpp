#include "Util.h"
#include <stdarg.h>
#pragma warning(disable:4244)

namespace util
{
	void output(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
#pragma warning(disable: 4996)
		vsnprintf(fdk::getStaticBuffer(), fdk::STATIC_BUFFER_SIZE, fmt, ap);
#pragma warning(default: 4996)
		va_end(ap);
		std::cout << fdk::getStaticBuffer() << std::endl;
		g_HGE->System_Log(fdk::getStaticBuffer());
	}

	void fillCell(const CellCoord& coord, DWORD color)
	{
		g_HGE.Rectangle(coord.x*CELL_SIZE_X, coord.y*CELL_SIZE_Y,
			(coord.x+1)*CELL_SIZE_X-1, (coord.y+1)*CELL_SIZE_Y-1, color);
	}

	CellCoord locationToCellCoord(const Location& location)
	{
		return CellCoord(
			static_cast<CellCoord::ValueType>(location.x/CELL_SIZE_X), 
			static_cast<CellCoord::ValueType>(location.y/CELL_SIZE_Y)
			);
	}

	Location cellCoordToLocation(const CellCoord& cellCoord)
	{
		return Location(
			static_cast<Location::ValueType>(cellCoord.x)*CELL_SIZE_X, 
			static_cast<Location::ValueType>(cellCoord.y)*CELL_SIZE_Y
			);
	}

	CellRange locationRangeToCellRange(const LocationRange& locationRange)
	{
		return CellRange(
			locationToCellCoord(locationRange.topLeft),
			locationToCellCoord(locationRange.bottomRight)
			);
	}

	LocationRange cellRangeToLocationRange(const CellRange& cellRange)
	{
		return LocationRange(
			cellCoordToLocation(cellRange.topLeft),
			cellCoordToLocation(cellRange.bottomRight)
			);
	}

	VertexCoord locationToVertexCoord(const Location& location)
	{
		return VertexCoord(
			static_cast<CellCoord::ValueType>( (location.x-HALF_CELL_SIZE_X)/HALF_CELL_SIZE_X ), 
			static_cast<CellCoord::ValueType>( (location.y-HALF_CELL_SIZE_Y)/HALF_CELL_SIZE_Y )
			);
	}

	Location vertexCoordToLocation(const VertexCoord& vertexCoord)
	{
		return Location(
			static_cast<Location::ValueType>(vertexCoord.x+1)*HALF_CELL_SIZE_X, 
			static_cast<Location::ValueType>(vertexCoord.y+1)*HALF_CELL_SIZE_Y
			);
	}
}
