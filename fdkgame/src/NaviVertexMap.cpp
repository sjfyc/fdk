#include <fdkgame/NaviVertexMap.h>
#include <fdkgame/NaviBlockMap.h>

namespace fdk { namespace game { namespace navi
{
	VertexMap::VertexMap(BlockMap& blockMap, UnitSize unitSize)
		: m_blockMap(blockMap)
		, m_unitSize(unitSize)
	{
		FDK_ASSERT(unitSize >= 1);
	}

	VertexMap::~VertexMap()
	{
	}

	void VertexMap::rebuildFromBlockMap()
	{
		m_data.reset(m_blockMap.getSizeX()*2-1, m_blockMap.getSizeY()*2-1);
		for (int y = 0; y < (int)m_blockMap.getSizeY(); ++y)
		{
			for (int x = 0; x < (int)m_blockMap.getSizeX(); ++x)
			{
				CellCoord cellCoord = CellCoord(x, y);
				if (m_blockMap.isBlock(cellCoord))
				{
					onSetBlock(cellCoord, true);
				}
			}
		}
	}

	void VertexMap::onSetBlock(const CellCoord& cellCoord, bool bSet)
	{
		for (UnitSize y = -m_unitSize; y <= m_unitSize; ++y)
		{
			for (UnitSize x = -m_unitSize; x <= m_unitSize; ++x)
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
