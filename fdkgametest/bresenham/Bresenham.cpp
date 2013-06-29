#include "Bresenham.h"
#include "Util.h"
#include "Types.h"

void Bresenham::draw(int x0, int y0, int x1, int y1)
{
	int startX = x0;
	int startY = y0;
	int targetX = x1;
	int targetY = y1;

	if (startX == targetX &&
		startY == targetY)
	{
		return;
	}

	enum
	{
		DependOnX,
		DependOnY,
	} state;

	int x = startX;
	int y = startY;

	int dx;
	int sx;
	if( targetX - startX > 0)
	{
		dx = targetX - startX;
		sx = 1;
	}
	else if(targetX - startX < 0)
	{
		dx = startX - targetX;
		sx = -1;
	}
	else
	{
		dx = 0;
		sx = 0;
	}
	int dy;
	int sy;
	if( targetY - startY > 0)
	{
		dy = targetY - startY;
		sy = 1;
	}
	else if(targetY - startY < 0)
	{
		dy = startY - targetY;
		sy = -1;
	}
	else
	{
		dy = 0;
		sy = 0;
	}

	if(dy > dx)
	{
		std::swap(dy,dx);
		state = DependOnY;
	}
	else
	{
		state = DependOnX;
	}
	//initialize the error term to compensate for a nonezero intercept
	int NError = 2 * dy - dx;
	for (int i = 1; i <= dx; ++i)
	{
		CellCoord vertexCoord = CellCoord(x, y);
		util::fillCell(vertexCoord, ARGB(255, 200,200, 200));

		if (NError >= 0)
		{
			if (state == DependOnY) 
			{
				CellCoord vertexCoord = CellCoord(x, y);
				util::fillCell(vertexCoord, ARGB(255, 200,200, 200));

				x = x + sx;
				vertexCoord = CellCoord(x, y);
				util::fillCell(vertexCoord, ARGB(255, 200,200, 200));

			}
			else
			{
				vertexCoord.reset(x+sx, y);
				vertexCoord = CellCoord(x, y);
				util::fillCell(vertexCoord, ARGB(255, 200,200, 200));

				y = y + sy;
				vertexCoord.reset(x, y);
				vertexCoord = CellCoord(x, y);
				util::fillCell(vertexCoord, ARGB(255, 200,200, 200));
			}
			NError = NError - 2 * dx;
		}
		if (state == DependOnY)
		{
			y = y + sy;
		}
		else
		{
			x = x + sx;
		}
		NError = NError + 2 * dy;
	}
}
