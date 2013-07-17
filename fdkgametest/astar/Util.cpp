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

	static double getSecondsPerCycle()
	{
		LARGE_INTEGER frequency;
		BOOL result = QueryPerformanceFrequency(&frequency);
		FDK_ASSERT(result);
		return 1.0 / frequency.QuadPart;
	}

	double getSeconds()
	{
		static double secondsPerCycle = getSecondsPerCycle();
		LARGE_INTEGER cycles;
		QueryPerformanceCounter(&cycles);
		// Add big number to make bugs apparent where return value is being passed to FLOAT
		return cycles.QuadPart * secondsPerCycle + 16777216.0;
	}
}
