#include "AStar.h"
#include "Util.h"
#include "Game.h"
#include "Board.h"
#include "Font.h"
#include "Option.h"

void AStarRecorder::onOpenNode(const fdkgame::navi::Environment& env, int nodeID, int parentNodeID, bool bReopen)
{
	CellCoord cellCoord = g_Board.toNodeCoord(nodeID);
	m_openCells.insert(cellCoord);
}

void AStarRecorder::onCloseNode(const fdkgame::navi::Environment& env, int nodeID)
{
	CellCoord cellCoord = g_Board.toNodeCoord(nodeID);
	m_openCells.erase(cellCoord);
	m_closeCells.insert(cellCoord);
	m_currentClosedCell = cellCoord;
}

void AStarRecorder::render()
{
	// 绘制open表
	for (Cells::const_iterator it = m_openCells.begin(); it != m_openCells.end(); ++it)
	{
		const CellCoord& cellCoord = *it;
		if (cellCoord == g_Game.getStartCoord() || cellCoord == g_Game.getTargetCoord())
		{
			continue;
		}
		util::fillCell(cellCoord, ARGB(255, 128, 128, 255));
	}
	// 绘制close表
	for (Cells::const_iterator it = m_closeCells.begin(); it != m_closeCells.end(); ++it)
	{
		const CellCoord& cellCoord = *it;
		if (cellCoord == g_Game.getStartCoord() || cellCoord == g_Game.getTargetCoord())
		{
			continue;
		}
		util::fillCell(cellCoord, ARGB(255, 210, 248, 207));
	}	
	// 绘制当前关闭节点
	if (!(m_currentClosedCell == g_Game.getStartCoord() || m_currentClosedCell == g_Game.getTargetCoord()) )
	{
		util::fillCell(m_currentClosedCell, MyColor_Yellow);
	}	
}

AStar::AStar(int startNodeID, int targetNodeID)
{
	if (g_Option.getNavigatorType() == Option::NavigatorType_AStar)
	{
		m_navigator = new fdkgame::navi::AStar(g_Board, startNodeID, targetNodeID);
	}
	else if (g_Option.getNavigatorType() == Option::NavigatorType_Jps)
	{
		m_navigator = new fdkgame::navi::Jps(g_Board, startNodeID, targetNodeID);
	}

	m_navigator->setRecorder(this);
}

AStar::~AStar()
{
	delete m_navigator;
}

void AStar::render()
{
	_Recorder::render();
	// 绘制路径
	if (m_navigator->getSearchResult() == Navigator::SearchResult_Completed)
	{
		CellCoord prevCellCoord = g_Game.getStartCoord();
		const std::vector<int>& path = m_navigator->getPath();
		for (int i = (int)path.size()-1; i >= 0; --i)
		{
			Location prevCenterLocation = util::cellCoordToLocation(prevCellCoord);
			prevCenterLocation += Location(CELL_SIZE_X/2, CELL_SIZE_Y/2);

			CellCoord currentCellCoord = g_Board.toNodeCoord(path[i]);
			Location currentCenterLocation = util::cellCoordToLocation(currentCellCoord);
			currentCenterLocation += Location(CELL_SIZE_X/2, CELL_SIZE_Y/2);

			g_HGE->Gfx_RenderLine(prevCenterLocation.x, prevCenterLocation.y, 
				currentCenterLocation.x, currentCenterLocation.y,
				MyColor_Blue
				);
			prevCellCoord = currentCellCoord;
		}
		g_Font.printf(2, 2, HGETEXT_LEFT, "path total cost: %d", m_navigator->getPathCost());
	}	
}
