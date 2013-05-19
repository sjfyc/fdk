#include "Util.h"
#pragma warning(disable:4244)

namespace util
{
	void fillCell(const CellCoord& coord, DWORD color)
	{
		g_HGE.Rectangle(coord.x*CELL_SIZE_X, coord.y*CELL_SIZE_Y,
			(coord.x+1)*CELL_SIZE_X-1, (coord.y+1)*CELL_SIZE_Y-1, color);
	}
}
