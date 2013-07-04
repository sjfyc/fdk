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
		
		ColorType color = 0;
		for (int y = 0; y < sizeY; ++y)
		{
			for (int x = 0; x < sizeX; ++x)
			{
				CellCoord cur(x, y);
				if (m_outer.isNodeWithCoordReachable(cur) &&
					m_colors(x, y) == UNCOLORED)
				{
					floodFill(cur, color++);
				}
			}
		}
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
				if (m_outer.isValidNodeCoord(neighbour) &&
					m_outer.isNodeWithCoordReachable(neighbour) &&
					m_colors(neighbour.x, neighbour.y) == UNCOLORED)
				{
					pending.push(neighbour); // 斜角的节点可能被重复添加
				}
			}
		}
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
					for (std::set<NodeCoord>::iterator itNode = aroundConnector->occupiedNodes.begin(); itNode != aroundConnector->occupiedNodes.end(); ++itNode)
					{
						const NodeCoord& occpiedNode = *itNode;
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
				unoccupyNode(*connector, cur);
				occupyNode(*newConnector, cur);
				bool result = handled.insert(cur).second;
				FDK_ASSERT(result);

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
}}}
