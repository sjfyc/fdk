#ifndef __FDKGAME_NAVIBASE_H_INCLUDE__
#define __FDKGAME_NAVIBASE_H_INCLUDE__
#include "Base.h"
#include <vector>
#include <set>
#include <map>
#include <fdk/Array2D.h>
#include "Math.h"
namespace fdk { namespace game { namespace navi
{
	class Environment;
	class Navigator;
	class GridEnv;
	class GridEnvColorComponent;
	class GridEnvConnectorComponent;
	typedef Vector2D<int> GridNodeCoord;

	const int INVALID_NODEID = -1;	
	const int COST_STRAIGHT = 100;
	const int COST_DIAGONAL = 142;
	const int PATHUNEXIST_COST = -1;

	struct SuccessorNodeInfo
	{
		int nodeID;
		int cost;
	};	

	// 环境要求：所有节点ID必须从0开始并依次自增
	class FDKGAME_API Environment
	{
	public:
		virtual ~Environment() {}
		virtual bool isValidNodeID(int nodeID) const;
		virtual int getNodeSpaceSize() const = 0;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const = 0;
		virtual void getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const = 0;		
		virtual const GridEnv* toGridEnv() const;
		virtual GridEnv* toGridEnv();
	};

	class FDKGAME_API Navigator
	{
	protected:
		Navigator();
		virtual ~Navigator();
	};
		
	class FDKGAME_API GridEnv
		: public Environment
	{
	public:
		typedef GridNodeCoord NodeCoord;
		virtual int getSizeX() const = 0;
		virtual int getSizeY() const = 0;
		virtual bool isValidNodeCoord(const NodeCoord& nodeCoord) const;
		virtual bool isNodeReachable(int nodeID) const = 0;

		int toNodeID(const NodeCoord& nodeCoord) const;
		NodeCoord toNodeCoord(int nodeID) const;
		bool isNodeWithCoordReachable(const NodeCoord& nodeCoord) const;

		GridEnvColorComponent* getColorComponent() const;
		GridEnvConnectorComponent* getConnectorComponent() const;

		// Environment interfaces
		virtual int getNodeSpaceSize() const;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const;
		virtual const GridEnv* toGridEnv() const;
		virtual GridEnv* toGridEnv();
	protected:
		GridEnv();
		virtual ~GridEnv();
		int getHeuristicManhattan(int startNodeID, int targetNodeID) const;
		int getHeuristicChebyshev(int startNodeID, int targetNodeID) const;
		int getHeuristicEuclidean(int startNodeID, int targetNodeID) const;
		void getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result, bool bCutCorner) const;
		GridEnvColorComponent* m_colorComponent;
		GridEnvConnectorComponent* m_connectorComponent;
	private:
		bool tryAddSuccessorNode(Navigator& navigator, std::vector<SuccessorNodeInfo>& result, const NodeCoord& nodeCoord, int cost, int parentNodeID) const;
	};
	
	class FDKGAME_API GridPartEnv
		: public GridEnv
	{		
	public:
		typedef Rect<NodeCoord::ValueType> Range;
		GridPartEnv(const GridEnv& outer, const Range& range);
		const GridEnv& getOuter() const;
		const Range& getRange() const;
		// GridEnv interfaces
		virtual int getSizeX() const;
		virtual int getSizeY() const;
		virtual bool isValidNodeCoord(const NodeCoord& nodeCoord) const;
		virtual bool isNodeReachable(int nodeID) const;		
	private:
		const GridEnv& m_outer;
		Range m_range;
	};

	class FDKGAME_API GridEnvColorComponent
	{
	public:
		typedef GridNodeCoord NodeCoord;
		typedef int ColorType;
		typedef Array2D<ColorType> ColorMap;		
		static const ColorType UNCOLORED = -1;
		GridEnvColorComponent(const GridEnv& outer);
		~GridEnvColorComponent();
		void refill();
		const GridEnv& getOuter() const;
		const ColorMap& getColorMap() const;
		ColorType getColor(const NodeCoord& nodeCoord) const;
	private:
		void floodFill(const NodeCoord& nodeCoord, ColorType color);
		const GridEnv& m_outer;
		ColorMap m_colors;
	};
	
	// 对于已经着色的地图,墙体部分可以动态增删
	class FDKGAME_API GridEnvConnectorComponent
	{
	public:	
		typedef GridNodeCoord NodeCoord;
		struct Connector
		{
			std::set<GridEnvColorComponent::ColorType> connectedColors;
			std::set<NodeCoord> occupiedNodes;
			bool isConnected(GridEnvColorComponent::ColorType a, GridEnvColorComponent::ColorType b) const;
		};
		typedef std::set<Connector*> Connectors;
		GridEnvConnectorComponent(const GridEnvColorComponent& colorComponent);
		~GridEnvConnectorComponent();
		// 注意墙体必须先移除才能再增加
		void removeWall(const NodeCoord& nodeCoord);
		void addWall(const NodeCoord& nodeCoord);
		bool isConnected(const NodeCoord& a, const NodeCoord& b) const;
		const Connectors& getConnectors() const;
	private:		
		void occupyNode(Connector& connector, const NodeCoord& nodeCoord);
		void unoccupyNode(Connector& connector, const NodeCoord& nodeCoord);		
		const GridEnv& m_env;
		const GridEnvColorComponent& m_colorComponent;
		Connectors m_connectors;
		Array2D<Connector*> m_node2connector;
	};

	inline bool Environment::isValidNodeID(int nodeID) const
	{
		return nodeID >= 0 && nodeID < getNodeSpaceSize();
	}

	inline Navigator::Navigator()
	{
	}

	inline Navigator::~Navigator()
	{
	}

	inline int GridEnv::toNodeID(const NodeCoord& nodeCoord) const
	{
		FDK_ASSERT(isValidNodeCoord(nodeCoord));
		return nodeCoord.y * getSizeX() + nodeCoord.x;
	}

	inline GridEnv::NodeCoord GridEnv::toNodeCoord(int nodeID) const
	{
		FDK_ASSERT(isValidNodeID(nodeID));
		return NodeCoord(nodeID % getSizeX(), nodeID / getSizeX());
	}

	inline bool GridEnv::isNodeWithCoordReachable(const NodeCoord& nodeCoord) const
	{
		return isNodeReachable(toNodeID(nodeCoord));
	}

	inline GridEnvColorComponent* GridEnv::getColorComponent() const
	{
		return m_colorComponent;
	}

	inline GridEnvConnectorComponent* GridEnv::getConnectorComponent() const
	{
		return m_connectorComponent;
	}

	inline GridPartEnv::GridPartEnv(const GridEnv& outer, const Range& range)
		: m_outer(outer)
		, m_range(range)
	{
	}

	inline const GridEnv& GridPartEnv::getOuter() const
	{
		return m_outer;
	}

	inline const GridPartEnv::Range& GridPartEnv::getRange() const
	{
		return m_range;
	}

	inline const GridEnv& GridEnvColorComponent::getOuter() const
	{
		return m_outer;
	}

	inline const GridEnvColorComponent::ColorMap& GridEnvColorComponent::getColorMap() const
	{
		return m_colors;
	}
	
	inline GridEnvColorComponent::ColorType GridEnvColorComponent::getColor(const NodeCoord& nodeCoord) const
	{
		return m_colors(nodeCoord.x, nodeCoord.y);
	}

	inline const GridEnvConnectorComponent::Connectors& GridEnvConnectorComponent::getConnectors() const
	{
		return m_connectors;
	}
}}}

#endif
