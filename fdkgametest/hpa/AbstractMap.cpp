#include "AbstractMap.h"
#include "Util.h"
#include "Board.h"
#pragma warning(disable:4244)

AbstractMap::AbstractMap()
	: _Base(g_Board, fdkgame::VectorI(8, 8))
{
	rebuildAbstract();
}

void AbstractMap::draw()
{
	for (size_t i = 0; i < m_clusters.count(); ++i)
	{
		const Cluster& cluster = *m_clusters.raw_data()[i];
		CellCoord topLeftCellCoord;
		topLeftCellCoord.x = cluster.getClusterCoord().x*getMaxClusterSize().x;
		topLeftCellCoord.y = cluster.getClusterCoord().y*getMaxClusterSize().y;
		g_HGE.FrameRect(topLeftCellCoord.x*CELL_SIZE_X, topLeftCellCoord.y*CELL_SIZE_Y,
			(topLeftCellCoord.x+cluster.getRange().width())*CELL_SIZE_X, (topLeftCellCoord.y+cluster.getRange().height())*CELL_SIZE_Y, 
			ARGB(255, 45, 151, 128), 2);
	}

	for (size_t i = 0; i < m_entrances.size(); ++i)
	{
		const Entrance& entrance = m_entrances[i];
		CellCoord node1CellCoord = m_lowLevelMap.getNodeCoord(entrance.lowLevelNode1ID);
		CellCoord node2CellCoord = m_lowLevelMap.getNodeCoord(entrance.lowLevelNode2ID);
		util::fillCell(node1CellCoord, ARGB(255, 210, 248, 207));
		util::fillCell(node2CellCoord, ARGB(255, 210, 248, 207));
		g_HGE->Gfx_RenderLine(
			node1CellCoord.x*CELL_SIZE_X+CELL_SIZE_X/2,
			node1CellCoord.y*CELL_SIZE_X+CELL_SIZE_X/2,
			node2CellCoord.x*CELL_SIZE_X+CELL_SIZE_X/2,
			node2CellCoord.y*CELL_SIZE_X+CELL_SIZE_X/2,
			ARGB(255, 57, 92, 145)
			);
	}
}
