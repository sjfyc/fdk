#include <fdkgame/NaviBase.h>
#include <stack>
#include <algorithm>
#include <iterator>
#include <fdkgame/NaviTypes.h>

namespace fdk { namespace game { namespace navi
{
	template <class SetT>
	bool isIntersect(const SetT& a, const SetT& b)
	{
		SetT c;
		std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()) );
		return !c.empty();
	}

	const GridEnv* Environment::toGridEnv() const
	{
		return 0;
	}

	GridEnv* Environment::toGridEnv()
	{
		return 0;
	}


	bool GridEnv::isValidNodeCoord(const NodeCoord& nodeCoord) const
	{
		return nodeCoord.x >= 0 && nodeCoord.x < getSizeX()
			&& nodeCoord.y >= 0 && nodeCoord.y < getSizeY();
	}

	int GridEnv::getNodeSpaceSize() const
	{
		return getSizeX() * getSizeY();
	}
	
	void GridEnv::getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		getSuccessorNodes(navigator, nodeID, result, false);
	}

	inline bool cutCornerOp(bool bCutCorner, bool a, bool b)
	{
		return bCutCorner ? (a && b) : (a || b);
	}
	void GridEnv::getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result, bool bCutCorner) const
	{
		NodeCoord coord = toNodeCoord(nodeID);

		const bool bLeft = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x-1,coord.y), COST_STRAIGHT, nodeID);		
		const bool bTop = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x,coord.y-1), COST_STRAIGHT, nodeID);
		const bool bRight = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x+1,coord.y), COST_STRAIGHT, nodeID);
		const bool bBottom = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x,coord.y+1), COST_STRAIGHT, nodeID);

		if (cutCornerOp(bCutCorner, bLeft, bTop))
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x-1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (cutCornerOp(bCutCorner, bTop, bRight))
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x+1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (cutCornerOp(bCutCorner, bRight, bBottom))
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x+1,coord.y+1), COST_DIAGONAL, nodeID);
		}
		if (cutCornerOp(bCutCorner, bBottom, bLeft))
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x-1,coord.y+1), COST_DIAGONAL, nodeID);
		}
	}

	bool GridEnv::tryAddSuccessorNode(Navigator& navigator, std::vector<SuccessorNodeInfo>& result, const NodeCoord& nodeCoord, int cost, int parentNodeID) const
	{
		if (!isValidNodeCoord(nodeCoord))
		{
			return false;
		}		
		const int nodeID = toNodeID(nodeCoord);
		if (!isNodeReachable(nodeID))
		{
			return false;
		}

		//if (navigator.getEnvironmentChecker() &&
		//	!navigator.getEnvironmentChecker()->checkSuccessorNode(*this, nodeID, parentNodeID))
		//{
		//	return false;
		//}

		SuccessorNodeInfo info;
		info.nodeID = nodeID;
		info.cost = cost;
		result.push_back(info);
		return true;
	}

	int GridEnv::getHeuristic(int startNodeID, int targetNodeID) const
	{
		return getHeuristicEuclidean(startNodeID, targetNodeID);
	}

	int GridEnv::getHeuristicManhattan(int startNodeID, int targetNodeID) const
	{
		NodeCoord startNodeCoord = toNodeCoord(startNodeID);
		NodeCoord targetNodeCoord = toNodeCoord(targetNodeID);

		NodeCoord startToTarget = targetNodeCoord - startNodeCoord;
		return COST_STRAIGHT * (abs(startToTarget.x) + abs(startToTarget.y));
	}

	int GridEnv::getHeuristicChebyshev(int startNodeID, int targetNodeID) const
	{
		NodeCoord startNodeCoord = toNodeCoord(startNodeID);
		NodeCoord targetNodeCoord = toNodeCoord(targetNodeID);

		return COST_STRAIGHT * maxOf(abs(startNodeCoord.x - targetNodeCoord.x), abs(startNodeCoord.y - targetNodeCoord.y));
	}

	int GridEnv::getHeuristicEuclidean(int startNodeID, int targetNodeID) const
	{
		NodeCoord startNodeCoord = toNodeCoord(startNodeID);
		NodeCoord targetNodeCoord = toNodeCoord(targetNodeID);

		NodeCoord startToTarget = targetNodeCoord - startNodeCoord;
		return COST_STRAIGHT * startToTarget.length();
	}
	
	const GridEnv* GridEnv::toGridEnv() const
	{
		return this;
	}

	GridEnv* GridEnv::toGridEnv()
	{
		return this;
	}
	
	GridEnv::GridEnv()
		: m_colorComponent(0)
		, m_connectorComponent(0)
	{
	}

	GridEnv::~GridEnv()
	{
		FDK_ASSERT(!m_colorComponent);
		FDK_ASSERT(!m_connectorComponent);
	}

	int GridPartEnv::getSizeX() const
	{
		return m_outer.getSizeX();
	}

	int GridPartEnv::getSizeY() const
	{
		return m_outer.getSizeY();
	}

	bool GridPartEnv::isValidNodeCoord(const NodeCoord& nodeCoord) const
	{
		return m_range.contain(nodeCoord) && m_outer.isValidNodeCoord(nodeCoord);
	}

	bool GridPartEnv::isNodeReachable(int nodeID) const
	{
		return m_outer.isNodeReachable(nodeID);
	}

	GridEnvColorComponent::GridEnvColorComponent(const GridEnv& outer)
		: m_outer(outer)
		, m_mainColor(UNCOLORED)
	{
	}

	GridEnvColorComponent::~GridEnvColorComponent()
	{
	}

	void GridEnvColorComponent::refill()
	{
		const int sizeX = m_outer.getSizeX();
		const int sizeY = m_outer.getSizeY();
		m_colors.reset(sizeX, sizeY, UNCOLORED);
		m_nodeCountWithColor.clear();
		m_mainColor = UNCOLORED;
		m_tempColors.reset(sizeX, sizeY, UNCOLORED);
		
		ColorType color = UNCOLORED;
		for (int y = 0; y < sizeY; ++y)
		{
			for (int x = 0; x < sizeX;)
			{
				CellCoord cur(x, y);
				if (isColorable(cur))
				{
					FDK_ASSERT(color <= MAX_COLOR_COUNT-1);
					int xMin, xMax;
					floodFillScanLine(cur, ++color, xMin, xMax, 0, false);
					x = xMax+1;
				}				
				else
				{
					++x;
				}
			}
		}
		updateMainColor();
	}

	void GridEnvColorComponent::floodFill(const NodeCoord& nodeCoord, ColorType color)
	{
		std::stack<NodeCoord> pending;
		pending.push(nodeCoord);
		while (!pending.empty())
		{
			NodeCoord cur = pending.top();
			pending.pop();
			if (m_colors(cur.x, cur.y) != UNCOLORED)
			{
				continue;
			}
			
			m_colors(cur.x, cur.y) = color;
			++m_nodeCountWithColor[color];

			const NodeCoord neighbours[] =
			{
				NodeCoord(cur.x-1, cur.y),
				NodeCoord(cur.x+1, cur.y),
				NodeCoord(cur.x, cur.y-1),
				NodeCoord(cur.x, cur.y+1),
			};

			for (size_t i = 0; i < FDK_DIM(neighbours); ++i)
			{
				const NodeCoord& neighbour = neighbours[i];
				if (isColorable(neighbour))
				{
					pending.push(neighbour); // 斜角的节点可能被重复添加
				}
			}
		}
	}

	void GridEnvColorComponent::floodFillScanLine(const NodeCoord& nodeCoord, ColorType color, int& xMin, int& xMax, std::pair<int, int>* parentRange, bool parentIsDown)
	{
		_floodFillScanLine(nodeCoord, color, xMin, xMax);

		bool bNeedScanDown = true;
		bool bNeedScanUp = true;
		if (parentRange)
		{
			if (parentIsDown)
			{
				if (xMin >= parentRange->first && xMax <= parentRange->second)
				{
					bNeedScanDown = false;
				}
			}
			else
			{
				if (xMin >= parentRange->first && xMax <= parentRange->second)
				{
					bNeedScanUp = false;
				}
			}
		}

		std::pair<int, int> NewParentRange;
		NewParentRange.first = xMin;
		NewParentRange.second = xMax;

		if (bNeedScanUp)
		{
			for (int x = xMin; x <= xMax;)
			{
				NodeCoord newNodeCoord(x, nodeCoord.y-1);
				if (isColorable(newNodeCoord))
				{
					int newXMin, newXMax;
					floodFillScanLine(newNodeCoord, color, newXMin, newXMax, &NewParentRange, true);
					x = newXMax+1;
				}
				else
				{
					++x;
				}
			}
		}

		if (bNeedScanDown)
		{
			for (int x = xMin; x <= xMax;)
			{
				NodeCoord newNodeCoord(x, nodeCoord.y+1);
				if (isColorable(newNodeCoord))
				{
					int newXMin, newXMax;
					floodFillScanLine(newNodeCoord, color, newXMin, newXMax, &NewParentRange, false);
					x = newXMax+1;
				}
				else
				{
					++x;
				}
			}
		}
	}

	void GridEnvColorComponent::_floodFillScanLine(const NodeCoord& nodeCoord, ColorType color, int& xMin, int& xMax)
	{
		m_colors(nodeCoord.x, nodeCoord.y) = color;
		NodeCoord cur(nodeCoord);
		xMin = nodeCoord.x;
		xMax = nodeCoord.x;
		while (1)
		{
			--cur.x;
			if (!m_outer.isValidNodeCoord(cur) ||
				!m_outer.isNodeWithCoordReachable(cur))
			{
				break;
			}
			ColorType& refColor = m_colors(cur.x, cur.y);
			FDK_ASSERT(refColor == UNCOLORED);
			refColor = color;
			xMin = cur.x;
		}
		cur = nodeCoord;
		while (1)
		{
			++cur.x;
			if (!m_outer.isValidNodeCoord(cur) ||
				!m_outer.isNodeWithCoordReachable(cur))
			{
				break;
			}
			ColorType& refColor = m_colors(cur.x, cur.y);
			FDK_ASSERT(refColor == UNCOLORED);
			refColor = color;
			xMax = cur.x;
		}
	}

	bool GridEnvColorComponent::isColorable(const NodeCoord& nodeCoord) const
	{
		return m_outer.isValidNodeCoord(nodeCoord) 
			&& m_outer.isNodeWithCoordReachable(nodeCoord)
			&& m_colors(nodeCoord.x, nodeCoord.y) == UNCOLORED;
	}

	int GridEnvColorComponent::getNodeCountWithColor(ColorType color) const
	{
		std::map<ColorType, int>::const_iterator it = m_nodeCountWithColor.find(color);
		if (it == m_nodeCountWithColor.end())
		{
			return 0;
		}
		return it->second;
	}

	void GridEnvColorComponent::updateMainColor()
	{
		int maxNodeCount = 0;
		for (std::map<ColorType, int>::iterator it = m_nodeCountWithColor.begin();
			it != m_nodeCountWithColor.end(); ++it)
		{
			const ColorType color = it->first;
			const int nodeCount = it->second;
			if (it == m_nodeCountWithColor.begin()) 
			{
				m_mainColor = color;
				maxNodeCount = nodeCount;
			}
			else
			{
				if (nodeCount > maxNodeCount)
				{
					maxNodeCount = nodeCount;
					m_mainColor = color;
				}
			}
		}
	}
	
	GridEnvColorComponent::ColorType GridEnvColorComponent::getColor(const NodeCoord& nodeCoord, bool bConsiderTempColor) const
	{
		if (bConsiderTempColor)
		{
			const ColorType tempColor = m_tempColors(nodeCoord.x, nodeCoord.y);
			if (tempColor != UNCOLORED)
			{
				return tempColor;
			}
		}		
		return m_colors(nodeCoord.x, nodeCoord.y);
	}

	void GridEnvColorComponent::clearTempColors()
	{
		memset(m_tempColors.raw_data(), UNCOLORED, m_tempColors.count()*sizeof(ColorType));
	}

	GridEnvConnectorComponent::GridEnvConnectorComponent(const GridEnvColorComponent& colorComponent)
		: m_env(colorComponent.getOuter())
		, m_colorComponent(colorComponent)
	{
	}

	GridEnvConnectorComponent::~GridEnvConnectorComponent()
	{
		clear();
	}

	void GridEnvConnectorComponent::clear()
	{
		for (Connectors::iterator it = m_connectors.begin(); it != m_connectors.end(); ++it)
		{
			Connector* connector = *it;
			delete connector;
		}
		m_connectors.clear();
	}

	void GridEnvConnectorComponent::removeWall(const NodeCoord& nodeCoord, bool autoUpdateConnecting)
	{
		FDK_ASSERT(m_colorComponent.getColor(nodeCoord) == GridEnvColorComponent::UNCOLORED);
		FDK_ASSERT(!getConnector(nodeCoord));

		const NodeCoord neighbours[4] = 
		{
			NodeCoord(nodeCoord.x-1, nodeCoord.y),
			NodeCoord(nodeCoord.x+1, nodeCoord.y),
			NodeCoord(nodeCoord.x, nodeCoord.y-1),
			NodeCoord(nodeCoord.x, nodeCoord.y+1),
		};
		
		std::set<ColorType> aroundColors;
		std::set<Connector*> aroundConnectors;
		for (size_t i = 0; i < FDK_DIM(neighbours); ++i)
		{
			const NodeCoord& neighbour = neighbours[i];			
			if (!m_env.isValidNodeCoord(neighbour))
			{
				continue;
			}
			Connector* aroundConnector = getConnectorForOperate(neighbour);
			if (aroundConnector)
			{// 存在连通器
				aroundConnectors.insert(aroundConnector);
			}
			else
			{// 不是连通器				
				ColorType color = m_colorComponent.getColor(neighbour);
				if (color == GridEnvColorComponent::UNCOLORED)
				{// 是墙体
					// DONOTHING
				}
				else
				{// 正常区域
					aroundColors.insert(color);
				}
			}
		}
		if (aroundConnectors.empty() || aroundConnectors.size() > 1)
		{// 周围不存在连通器则产生一个新的或者对多个连通器合并成新连通器
			Connector* newConnector = new Connector;			
			bool result = m_connectors.insert(newConnector).second;
			FDK_ASSERT(result);
			newConnector->connectedColors = aroundColors;
			occupyNode(*newConnector, nodeCoord);

			if (aroundConnectors.size() > 1)
			{
				// 把所有连通器合并
				for (Connectors::iterator it = aroundConnectors.begin(); it != aroundConnectors.end(); ++it)
				{
					Connector* aroundConnector = *it;
					newConnector->connectedColors.insert(aroundConnector->connectedColors.begin(), aroundConnector->connectedColors.end());
					while (!aroundConnector->occupiedNodes.empty())
					{
						const NodeCoord occpiedNode = *aroundConnector->occupiedNodes.begin();
						unoccupyNode(*aroundConnector, occpiedNode);
						occupyNode(*newConnector, occpiedNode);
					}
					result = m_connectors.erase(aroundConnector) > 0;
					FDK_ASSERT(result);
					delete aroundConnector;
				}
			}
		}
		else
		{
			Connector* aroundConnector = *aroundConnectors.begin();
			aroundConnector->connectedColors.insert(aroundColors.begin(), aroundColors.end());
			occupyNode(*aroundConnector, nodeCoord);
		}

		if (autoUpdateConnecting)
		{
			updateConnectingInfo();
		}		
	}

	void GridEnvConnectorComponent::addWall(const NodeCoord& nodeCoord, bool autoUpdateConnecting)
	{
		FDK_ASSERT(m_colorComponent.getColor(nodeCoord) == GridEnvColorComponent::UNCOLORED);

		Connector* connector = getConnectorForOperate(nodeCoord);
		FDK_ASSERT(connector);

		unoccupyNode(*connector, nodeCoord);
		
		while (!connector->occupiedNodes.empty())
		{
			Connector* newConnector = new Connector;
			m_connectors.insert(newConnector);

			std::stack<NodeCoord> pending;
			pending.push(*connector->occupiedNodes.begin());

			std::set<NodeCoord> handled;
			while (!pending.empty())
			{
				NodeCoord cur = pending.top();
				pending.pop();

				if (!handled.insert(cur).second)
				{
					continue;
				}

				unoccupyNode(*connector, cur);
				occupyNode(*newConnector, cur);

				const NodeCoord neighbours[4] = 
				{
					NodeCoord(cur.x-1, cur.y),
					NodeCoord(cur.x+1, cur.y),
					NodeCoord(cur.x, cur.y-1),
					NodeCoord(cur.x, cur.y+1),
				};
				for (size_t i = 0; i < FDK_DIM(neighbours); ++i)
				{
					const NodeCoord& neighbour = neighbours[i];
					if (!m_env.isValidNodeCoord(neighbour))
					{
						continue;
					}
					if (handled.find(neighbour) != handled.end())
					{
						continue;
					}
					if (getConnector(neighbour))
					{// 存在连通器
						pending.push(neighbour);
					}
					else
					{// 不是连通器				
						ColorType color = m_colorComponent.getColor(neighbour);
						if (color == GridEnvColorComponent::UNCOLORED)
						{// 是墙体
							// DONOTHING
						}
						else
						{// 正常区域
							newConnector->connectedColors.insert(color);
						}
					}
				}
			}			
		}		
		bool result = m_connectors.erase(connector) > 0;
		FDK_ASSERT(result);
		delete connector;

		if (autoUpdateConnecting)
		{
			updateConnectingInfo();
		}		
	}

	void GridEnvConnectorComponent::occupyNode(Connector& connector, const NodeCoord& nodeCoord)
	{
		FDK_ASSERT(!getConnector(nodeCoord));
		bool result = connector.occupiedNodes.insert(nodeCoord).second;
		FDK_ASSERT(result);
		result = m_node2connector.insert(std::make_pair(nodeCoord, &connector)).second;
		FDK_ASSERT(result);
	}

	void GridEnvConnectorComponent::unoccupyNode(Connector& connector, const NodeCoord& nodeCoord)
	{
		FDK_ASSERT(getConnector(nodeCoord) == &connector);
		bool result = connector.occupiedNodes.erase(nodeCoord) > 0;
		FDK_ASSERT(result);
		result = m_node2connector.erase(nodeCoord) > 0;
		FDK_ASSERT(result);
	}

	void GridEnvConnectorComponent::updateConnectingInfo()
	{
		m_connectingInfo.clear();

		std::vector<Connector*> left(m_connectors.begin(), m_connectors.end());
		std::vector<Connector*> pending;

		std::set<ColorType> singleConnecting;
		bool resetConnecting = true;

		while (1)
		{
			if (left.empty())
			{
				break;
			}
			for (size_t i = 0; i < left.size(); ++i)
			{
				Connector* connector = left[i];
				if (i == 0 && resetConnecting)
				{
					singleConnecting.insert(connector->connectedColors.begin(), connector->connectedColors.end());
					resetConnecting = false;
					continue;
				}
				if (isIntersect(singleConnecting, connector->connectedColors))
				{
					singleConnecting.insert(connector->connectedColors.begin(), connector->connectedColors.end());
				}
				else
				{
					pending.push_back(connector);
				}
			}
			if (pending.empty())
			{
				m_connectingInfo.push_back(singleConnecting);
				break;
			}
			if (pending.size() == left.size())
			{
				m_connectingInfo.push_back(singleConnecting);
				singleConnecting.clear();
				resetConnecting = true;
				pending.clear();
				continue;
			}
			left.swap(pending);
			pending.clear();
		}

	}

	bool GridEnvConnectorComponent::isConnected(const NodeCoord& a, const NodeCoord& b) const
	{
		if (a == b)
		{
			return true;
		}
		if (!m_env.isNodeWithCoordReachable(a) || !m_env.isNodeWithCoordReachable(b))
		{
			return false;
		}
		ColorType aColor = m_colorComponent.getColor(a);
		ColorType bColor = m_colorComponent.getColor(b);
		const Connector* aConnector = getConnector(a);
		const Connector* bConnector = getConnector(b);
		if (aColor != GridEnvColorComponent::UNCOLORED)
		{
			if (bColor != GridEnvColorComponent::UNCOLORED)
			{
				return isConnected(aColor, bColor);
			}
			else
			{
				if (bConnector)
				{
					return isConnected(aColor, *bConnector);
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			if (bColor != GridEnvColorComponent::UNCOLORED)
			{
				if (aConnector)
				{
					return isConnected(bColor, *aConnector);
				}
				else
				{
					return false;
				}
			}
			else
			{
				if (aConnector && bConnector)
				{
					return isConnected(*aConnector, *bConnector);
				}
				return false;
			}
		}	
	}

	bool GridEnvConnectorComponent::isConnected(ColorType a, ColorType b) const
	{
		FDK_ASSERT(a != GridEnvColorComponent::UNCOLORED);
		FDK_ASSERT(b != GridEnvColorComponent::UNCOLORED);
		if (a == b)
		{
			return true;
		}
		for (size_t i = 0; i < m_connectingInfo.size(); ++i)
		{
			const std::set<ColorType>& singleConnecting = m_connectingInfo[i];
			if (singleConnecting.find(a) != singleConnecting.end() &&
				singleConnecting.find(b) != singleConnecting.end())
			{
				return true;
			}
		}
		return false;
	}

	bool GridEnvConnectorComponent::isConnected(ColorType a, const Connector& b) const
	{
		for (std::set<ColorType>::const_iterator it = b.connectedColors.begin(); 
			it != b.connectedColors.end(); ++it)
		{
			if (isConnected(a, *it))
			{
				return true;
			}
		}
		return false;
	}

	bool GridEnvConnectorComponent::isConnected(const Connector& a, const Connector& b) const
	{
		for (std::set<ColorType>::const_iterator it = a.connectedColors.begin(); 
			it != a.connectedColors.end(); ++it)
		{
			if (isConnected(*it, b))
			{
				return true;
			}
		}
		return false;
	}

	const GridEnvConnectorComponent::Connector* GridEnvConnectorComponent::getConnector(const NodeCoord& nodeCoord) const
	{
		Node2Connector::const_iterator it = m_node2connector.find(nodeCoord);
		if (it == m_node2connector.end())
		{
			return 0;
		}
		return it->second;
	}

	GridEnvConnectorComponent::Connector* GridEnvConnectorComponent::getConnectorForOperate(const NodeCoord& nodeCoord)
	{
		return const_cast<Connector*>(
			static_cast<const GridEnvConnectorComponent*>(this)->getConnector(nodeCoord)
			);
	}

	int GridEnvConnectorComponent::getConnectedNodeCount(const NodeCoord& nodeCoord) const
	{
		ColorType color = m_colorComponent.getColor(nodeCoord);				
		if (color != GridEnvColorComponent::UNCOLORED)
		{
			return getConnectedNodeCount(color);
		}
		const Connector* connector = getConnector(nodeCoord);
		if (connector && !connector->connectedColors.empty())
		{
			return getConnectedNodeCount(*connector->connectedColors.begin());
		}
		return 0;
	}

	int GridEnvConnectorComponent::getConnectedNodeCount(ColorType color) const
	{	
		for (size_t i = 0; i < m_connectingInfo.size(); ++i)
		{
			const std::set<ColorType>& colors = m_connectingInfo[i];
			if (colors.find(color) == colors.end())
			{
				continue;
			}
			int result = 0;
			for (std::set<ColorType>::const_iterator it = colors.begin(); it != colors.end(); ++it)
			{
				ColorType c = *it;
				result += m_colorComponent.getNodeCountWithColor(c);
				// 不包括连通器所占的节点
			}
			return result;
		}
		return m_colorComponent.getNodeCountWithColor(color);
	}

}}}
