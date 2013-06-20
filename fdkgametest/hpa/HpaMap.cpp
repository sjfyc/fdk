#include "HpaMap.h"
#include "Util.h"
#include "Option.h"
#include "Board.h"
#include "Game.h"
#include "Font.h"
#pragma warning(disable:4244)

HpaMap::HpaMap()
	: _Base(g_Board, fdkgame::VectorI(8, 8))
{
	rebuildAbstract();
}

void HpaMap::draw()
{
	for (size_t i = 0; i < getClusters().count(); ++i)
	{
		const Cluster& cluster = *getClusters().raw_data()[i];
		CellCoord topLeftCellCoord;
		topLeftCellCoord.x = cluster.getClusterCoord().x*getMaxClusterSize().x;
		topLeftCellCoord.y = cluster.getClusterCoord().y*getMaxClusterSize().y;
		g_HGE.FrameRect(topLeftCellCoord.x*CELL_SIZE_X, topLeftCellCoord.y*CELL_SIZE_Y,
			(topLeftCellCoord.x+cluster.getRange().width())*CELL_SIZE_X, (topLeftCellCoord.y+cluster.getRange().height())*CELL_SIZE_Y, 
			ARGB(255, 45, 151, 128), 2);
	}

	const AbstractGraph::Nodes& nodes = getAbstractGraph().getNodes();	
	for (AbstractGraph::Nodes::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		const AbstractNode* node = *it;	
		if (node->getInfo().lowLevelNodeID == getLowLevelMap().getNodeID(g_Game.getStartCoord()) ||
			node->getInfo().lowLevelNodeID == getLowLevelMap().getNodeID(g_Game.getTargetCoord()))
		{
			continue;
		}
		CellCoord cellCoord = getLowLevelMap().getNodeCoord(node->getInfo().lowLevelNodeID);
		util::fillCell(cellCoord, ARGB(255, 210, 248, 207));

		if (g_Option.isOn(Option::Toggle_ShowPortID))
		{
			g_Font.printf(cellCoord.x*CELL_SIZE_X+2, cellCoord.y*CELL_SIZE_Y+2, HGETEXT_LEFT, "%d", 
				node->getID());
		}
	}
	const AbstractGraph::Edges& edges = getAbstractGraph().getEdges();
	for (AbstractGraph::Edges::const_iterator it = edges.begin(); it != edges.end(); ++it)
	{
		const AbstractEdge* edge = *it;	
		if (edge->getInfo().bIntra && !g_Option.isOn(Option::Toggle_ShowIntraEdge))
		{
			continue;
		}
		CellCoord startCellCoord = getLowLevelMap().getNodeCoord(edge->getStartNode().getInfo().lowLevelNodeID);
		CellCoord targetCellCoord = getLowLevelMap().getNodeCoord(edge->getTargetNode().getInfo().lowLevelNodeID);

		g_HGE->Gfx_RenderLine(
			startCellCoord.x*CELL_SIZE_X+CELL_SIZE_X/2,
			startCellCoord.y*CELL_SIZE_X+CELL_SIZE_X/2,
			targetCellCoord.x*CELL_SIZE_X+CELL_SIZE_X/2,
			targetCellCoord.y*CELL_SIZE_X+CELL_SIZE_X/2,
			ARGB(255, 57, 92, 145)
			);
	}

	if (g_Option.isOn(Option::Toggle_ShowIntraEdge))
	{
		g_Font.printf(20, 20, HGETEXT_LEFT, "total edges: %d", edges.size());
	}

	if (g_Option.isOn(Option::Toggle_ShowClusterCoord))
	{
		for (size_t i = 0; i < getClusters().count(); ++i)
		{
			const Cluster& cluster = *getClusters().raw_data()[i];
			CellCoord topLeftCellCoord;
			topLeftCellCoord.x = cluster.getClusterCoord().x*getMaxClusterSize().x;
			topLeftCellCoord.y = cluster.getClusterCoord().y*getMaxClusterSize().y;
		
			g_Font.printf(topLeftCellCoord.x*CELL_SIZE_X+2, topLeftCellCoord.y*CELL_SIZE_Y+2, HGETEXT_LEFT, "%d,%d", 
				cluster.getClusterCoord().x, 
				cluster.getClusterCoord().y);			
		}
	}
}

Hpa::Hpa(fdkgame::findpath::HpaMap& env, int startNodeID, int targetNodeID)
	: _Base(env, startNodeID, targetNodeID, g_Option.getUnitSize())
{
	m_roughPath = getRoughPath();
}

Hpa::~Hpa()
{
}

void Hpa::render()
{
	// 绘制抽象路径
	{
		CellCoord prevCellCoord = g_Game.getStartCoord();
		const std::vector<int>& path = m_roughPath;
		for (int i = (int)path.size()-1; i >= 0; --i)
		{
			Location prevCenterLocation = util::cellCoordToLocation(prevCellCoord);
			prevCenterLocation += Location(CELL_SIZE_X/2, CELL_SIZE_Y/2);

			CellCoord currentCellCoord = g_Board.getNodeCoord(path[i]);
			Location currentCenterLocation = util::cellCoordToLocation(currentCellCoord);
			currentCenterLocation += Location(CELL_SIZE_X/2, CELL_SIZE_Y/2);

			g_HGE->Gfx_RenderLine(prevCenterLocation.x, prevCenterLocation.y+1, 
				currentCenterLocation.x, currentCenterLocation.y+1,
				ARGB(255,113,95,47)
				);
			prevCellCoord = currentCellCoord;
		}
	}	

	// 绘制精化路径
	{
		CellCoord prevCellCoord = g_Game.getStartCoord();
		for (size_t i = 0; i < m_path.size(); ++i)
		{
			Location prevCenterLocation = util::cellCoordToLocation(prevCellCoord);
			prevCenterLocation += Location(CELL_SIZE_X/2, CELL_SIZE_Y/2);

			CellCoord currentCellCoord = g_Board.getNodeCoord(m_path[i]);
			Location currentCenterLocation = util::cellCoordToLocation(currentCellCoord);
			currentCenterLocation += Location(CELL_SIZE_X/2, CELL_SIZE_Y/2);

			g_HGE->Gfx_RenderLine(prevCenterLocation.x, prevCenterLocation.y, 
				currentCenterLocation.x, currentCenterLocation.y,
				MyColor_Blue
				);
			prevCellCoord = currentCellCoord;
		}
	}	
}

int Hpa::popNextPathNode()
{
	int node = _Base::popNextPathNode();
	if (node == fdkgame::findpath::INVALID_NODEID)
	{
		return node;
	}
	m_path.push_back(node);
	return node;
}
