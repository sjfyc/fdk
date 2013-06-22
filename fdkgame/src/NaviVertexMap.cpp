#include <fdkgame/NaviVertexMap.h>
#include <fdkgame/NaviBlockMap.h>

namespace fdk { namespace game { namespace navi
{
	VertexMap::VertexMap(int unitSize)
		: m_unitSize(unitSize)
	{
		FDK_ASSERT(unitSize >= 1);
	}

	VertexMap::~VertexMap()
	{
	}

	void VertexMap::rebuildFromBlockMap(const BlockMap& blockMap)
	{
		m_data.reset(blockMap.getSizeX()*2-1, blockMap.getSizeY()*2-1);
		for (int y = 0; y < (int)blockMap.getSizeY(); ++y)
		{
			for (int x = 0; x < (int)blockMap.getSizeX(); ++x)
			{
				CellCoord cellCoord = CellCoord(x, y);
				if (blockMap.isBlock(cellCoord))
				{
					onSetBlock(cellCoord, true);
				}
			}
		}
	}

	void VertexMap::onSetBlock(const CellCoord& cellCoord, bool bSet)
	{
		for (int y = -m_unitSize; y <= m_unitSize; ++y)
		{
			for (int x = -m_unitSize; x <= m_unitSize; ++x)
			{				
				VertexCoord vertexCoord(cellCoord.x*2+x, cellCoord.y*2+y);
				if (m_data.is_valid_index(vertexCoord.x, vertexCoord.y))
				{
					m_data(vertexCoord.x, vertexCoord.y) += (bSet ? 1 : -1);
					FDK_ASSERT(m_data(vertexCoord.x, vertexCoord.y) >= 0);
				}
			}
		}		
	}
}}}
