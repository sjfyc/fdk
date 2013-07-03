#include "FloodFill.h"
#include "Board.h"
#include <stack>

void floodFill(const CellCoord& cell, int areaID)
{
	std::stack<CellCoord> pending;
	pending.push(cell);
	while (!pending.empty())
	{
		CellCoord cur = pending.top();
		pending.pop();
	}
}
