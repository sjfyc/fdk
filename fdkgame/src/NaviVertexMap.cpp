#include <fdkgame/NaviVertexMap.h>
#include <fdkgame/NaviBlockMap.h>

namespace fdk { namespace game { namespace navi
{
	VertexMap::VertexMap(BlockMap& blockMap, UnitSize unitSize)
		: m_blockMap(blockMap)
		, m_unitSize(unitSize)
	{
		FDK_ASSERT(unitSize >= 1);

		m_colorComponent = new GridEnvColorComponent(*this);
		m_connectorComponent = new GridEnvConnectorComponent(*m_colorComponent);
	}

	VertexMap::~VertexMap()
	{
		FDK_DELETE(m_connectorComponent);
		FDK_DELETE(m_colorComponent);
	}

	void VertexMap::rebuildFromBlockMap()
	{
		m_data.reset(m_blockMap.getSizeX()*2-1, m_blockMap.getSizeY()*2-1);
		m_allowModify.reset(m_data.size_x(), m_data.size_y(), true);
		for (int y = 0; y < (int)m_blockMap.getSizeY(); ++y)
		{
			for (int x = 0; x < (int)m_blockMap.getSizeX(); ++x)
			{
				CellCoord cellCoord = CellCoord(x, y);
				if (m_blockMap.isBlock(cellCoord))
				{
					onSetBlock(cellCoord, true, false);
				}
			}
		}
		m_colorComponent->refill();
	}

	void VertexMap::onSetBlock(const CellCoord& cellCoord, bool bSet, bool bModBuildedWall)
	{
		bool bConnectingInfoChanged = false;
		for (UnitSize y = -m_unitSize; y <= m_unitSize; ++y)
		{
			for (UnitSize x = -m_unitSize; x <= m_unitSize; ++x)
			{				
				VertexCoord vertexCoord(cellCoord.x*2+x, cellCoord.y*2+y);
				if (!isValidNodeCoord(vertexCoord))
				{
					continue;
				}
				m_data(vertexCoord.x, vertexCoord.y) += (bSet ? 1 : -1);
				FDK_ASSERT(m_data(vertexCoord.x, vertexCoord.y) >= 0);

				if (bModBuildedWall)
				{
					if (bSet && m_data(vertexCoord.x, vertexCoord.y) == 1)
					{
						getConnectorComponent()->addWall(vertexCoord, false);
						bConnectingInfoChanged = true;
					}
					else if (!bSet && m_data(vertexCoord.x, vertexCoord.y) == 0)
					{
						getConnectorComponent()->removeWall(vertexCoord, false);
						bConnectingInfoChanged = true;
					}
				}
			}
		}
		if (bConnectingInfoChanged)
		{
			getConnectorComponent()->updateConnectingInfo();
		}
	}

	bool VertexMap::isNodeReachable(int nodeID) const
	{
		FDK_ASSERT(isValidNodeID(nodeID));
		return m_data.raw_data()[nodeID] <= 0;
	}

	void VertexMap::onPlotUnit(const VertexCoord& vertexCoord, UnitSize unitSize, bool bPlot)
	{
		UnitSize totalExtend = unitSize+m_unitSize-1;
		for (UnitSize y = -totalExtend; y <= totalExtend; ++y)
		{
			for (UnitSize x = -totalExtend; x <= totalExtend; ++x)
			{				
				VertexCoord aroundVertexCoord(vertexCoord.x+x, vertexCoord.y+y);
				if (!isValidNodeCoord(aroundVertexCoord))
				{
					continue;
				}

				if (!m_allowModify(aroundVertexCoord.x, aroundVertexCoord.y))
				{
					continue;
				}

				m_data(aroundVertexCoord.x, aroundVertexCoord.y) += (bPlot ? 1 : -1);
				FDK_ASSERT(m_data(aroundVertexCoord.x, aroundVertexCoord.y) >= 0);
			}
		}	
	}

	void VertexMap::allowModify(const VertexCoord& vertexCoord, UnitSize unitSize, bool bAllow)
	{
		UnitSize totalExtend = unitSize+m_unitSize;
		for (UnitSize y = -totalExtend; y <= totalExtend; ++y)
		{
			for (UnitSize x = -totalExtend; x <= totalExtend; ++x)
			{				
				VertexCoord aroundVertexCoord(vertexCoord.x+x, vertexCoord.y+y);
				if (isValidNodeCoord(aroundVertexCoord))
				{
					m_allowModify(aroundVertexCoord.x, aroundVertexCoord.y) = bAllow;
				}
			}
		}
	}
	
	int VertexMap::getSizeX() const
	{
		return (int)m_data.size_x();
	}

	int VertexMap::getSizeY() const
	{
		return (int)m_data.size_y();
	}
	
	void VertexMap::getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		return _Base::getSuccessorNodes(navigator, nodeID, result, true);
	}
}}}
