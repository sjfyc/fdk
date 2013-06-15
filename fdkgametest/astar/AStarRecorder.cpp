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
	m_currentClosedCell = cellCoord;
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
		util::fillCell(cellCoord, ARGB(255, 128, 128, 255));
	}
	for (Cells::const_iterator it = m_closeCells.begin(); it != m_closeCells.end(); ++it)
	{
		const CellCoord& cellCoord = *it;
		if (cellCoord == g_Game.m_startCoord || cellCoord == g_Game.m_targetCoord)
		{
			continue;
		}
		util::fillCell(cellCoord, ARGB(255, 210, 248, 207));
	}
	if (m_currentClosedCell != g_Game.m_startCoord && m_currentClosedCell != g_Game.m_targetCoord)
	{
		util::fillCell(m_currentClosedCell, MyColor_Yellow);
	}
}
