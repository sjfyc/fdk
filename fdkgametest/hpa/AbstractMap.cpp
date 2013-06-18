#include "AbstractMap.h"
#include "Util.h"
#include "Board.h"
#pragma warning(disable:4244)

AbstractMap::AbstractMap()
	: _Base(g_Board, fdkgame::VectorI(8, 8))
{
	buildEntrances();
}

void AbstractMap::draw()
{
	for (size_t i = 0; i < m_clusters.size(); ++i)
	{
		const fdkgame::findpath::Cluster& cluster = m_clusters[i];
		g_HGE.FrameRect(cluster.m_topLeftCellCoord.x*CELL_SIZE_X, cluster.m_topLeftCellCoord.y*CELL_SIZE_Y,
			(cluster.m_topLeftCellCoord.x+cluster.m_size.x)*CELL_SIZE_X, (cluster.m_topLeftCellCoord.y+cluster.m_size.y)*CELL_SIZE_Y, 
			ARGB(255, 45, 151, 128), 2);
	}

	for (size_t i = 0; i < m_entrances.size(); ++i)
	{
		const fdkgame::findpath::Entrance& entrance = m_entrances[i];
		CellCoord node1CellCoord = m_lowLevelMap.getNodeCoord(entrance.node1ID);
		CellCoord node2CellCoord = m_lowLevelMap.getNodeCoord(entrance.node2ID);
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
