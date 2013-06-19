#include "HpaMap.h"
#include "Util.h"
#include "Board.h"
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
		util::fillCell(getLowLevelMap().getNodeCoord(node->getInfo().lowLevelNodeID), ARGB(255, 210, 248, 207));
	}
	const AbstractGraph::Edges& edges = getAbstractGraph().getEdges();
	for (AbstractGraph::Edges::const_iterator it = edges.begin(); it != edges.end(); ++it)
	{
		const AbstractEdge* edge = *it;	
		if (edge->getInfo().bIntra)
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
}
