#include "AStarRecorder.h"
#include "Board.h"
#include "Util.h"
#include "Game.h"

void AStarRecorder::onOpenNode(int nodeID, int parentNodeID, bool bReopen)
{
	CellCoord cellCoord = g_Board.getNodeCoord(nodeID);
	m_openCells.insert(cellCoord);
}

void AStarRecorder::onCloseNode(int nodeID)
{
	CellCoord cellCoord = g_Board.getNodeCoord(nodeID);
	m_openCells.erase(cellCoord);
	m_closeCells.insert(cellCoord);
}

void AStarRecorder::render()
{
	for (Cells::const_iterator it = m_openCells.begin(); it != m_openCells.end(); ++it)
	{
		const CellCoord& cellCoord = *it;
		if (cellCoord == g_Game.m_startCoord || cellCoord == g_Game.m_targetCoord)
		{
			continue;
		}
		util::fillCell(cellCoord, 0xff00ff00);
	}
	for (Cells::const_iterator it = m_closeCells.begin(); it != m_closeCells.end(); ++it)
	{
		const CellCoord& cellCoord = *it;
		if (cellCoord == g_Game.m_startCoord || cellCoord == g_Game.m_targetCoord)
		{
			continue;
		}
		util::fillCell(cellCoord, 0xffff0000);
	}
}
