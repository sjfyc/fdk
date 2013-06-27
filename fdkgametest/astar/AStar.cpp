#include "AStar.h"
#include "Util.h"
#include "Game.h"
#include "Board.h"
#include "Font.h"
#include "Option.h"

AStarRecorder::AStarRecorder()
	: m_nextOpenSerial(0)
{
}

void AStarRecorder::onOpenNode(const fdkgame::navi::Environment& env, int nodeID, int parentNodeID, bool bReopen)
{
	RecNodeInfo recinfo;
	recinfo.cellCoord = g_Board.toNodeCoord(nodeID);
	recinfo.parentNodeID = parentNodeID;
	recinfo.openSerial = m_nextOpenSerial++;
	m_openCells.insert(recinfo);
}

void AStarRecorder::onCloseNode(const fdkgame::navi::Environment& env, int nodeID)
{
	CellCoord cellCoord = g_Board.toNodeCoord(nodeID);
	RecNodeInfo search;
	search.cellCoord = cellCoord;
	OpenCells::iterator it = m_openCells.find(search);
	FDK_ASSERT(it != m_openCells.end());
	RecNodeInfo recinfo = *it;
	m_openCells.erase(it);
	m_closeCells.push_back(recinfo);
	m_currentClosedCell = cellCoord;
}

void AStarRecorder::render()
{
	// 绘制open表
	for (OpenCells::const_iterator it = m_openCells.begin(); it != m_openCells.end(); ++it)
	{
		const RecNodeInfo& recInfo = *it;
		const CellCoord& cellCoord = recInfo.cellCoord;
		if (cellCoord == g_Game.getStartCoord() || cellCoord == g_Game.getTargetCoord())
		{
			continue;
		}
		util::fillCell(cellCoord, ARGB(255, 128, 128, 255));
		g_Font.printf(cellCoord.x*CELL_SIZE_X+2.0f,cellCoord.y*CELL_SIZE_Y+2.0f,HGETEXT_LEFT,"%d",recInfo.openSerial);
	}
	// 绘制close表
	for (size_t i = 0; i < m_closeCells.size(); ++i)
	{
		const RecNodeInfo& recInfo = m_closeCells[i];
		const CellCoord& cellCoord = recInfo.cellCoord;
		if (cellCoord == g_Game.getStartCoord() || cellCoord == g_Game.getTargetCoord())
		{
			continue;
		}
		util::fillCell(cellCoord, ARGB(255, 210, 248, 207));
		g_Font.printf(cellCoord.x*CELL_SIZE_X+2.0f,cellCoord.y*CELL_SIZE_Y+2.0f,HGETEXT_LEFT,"%d", recInfo.openSerial);
		g_Font.printf((cellCoord.x+1)*CELL_SIZE_X-20.0f,(cellCoord.y+1)*CELL_SIZE_Y-10.0f,HGETEXT_LEFT,"%d", i);
		
		if (recInfo.parentNodeID != fdkgame::navi::INVALID_NODEID)
		{
			CellCoord parentCellCoord = g_Board.toNodeCoord(m_closeCells[i].parentNodeID);
			g_HGE->Gfx_RenderLine(cellCoord.x*CELL_SIZE_X+CELL_SIZE_X/2.0f, cellCoord.y*CELL_SIZE_Y+CELL_SIZE_Y/2.0f, 
				parentCellCoord.x*CELL_SIZE_X+CELL_SIZE_X/2.0f, parentCellCoord.y*CELL_SIZE_Y+CELL_SIZE_Y/2.0f,
				ARGB(220, 78, 116, 148)
				);
		}				
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
				ARGB(255, 132, 38, 64)
				);
			g_HGE->Gfx_RenderLine(prevCenterLocation.x, prevCenterLocation.y+1,
				currentCenterLocation.x, currentCenterLocation.y+1,
				ARGB(255, 132, 38, 64)
				);
			prevCellCoord = currentCellCoord;
		}
		g_Font.printf(2, 2, HGETEXT_LEFT, "path total cost: %d", m_navigator->getPathCost());
	}	
}
