#include <fdkgame/NaviBase.h>

namespace fdk { namespace game { namespace navi
{
	bool Environment::isValidNodeID(int nodeID) const
	{
		return nodeID >= 0 && nodeID < getNodeSpaceSize();
	}

	int GridBasedEnv::toNodeID(const NodeCoord& nodeCoord) const
	{
		FDK_ASSERT(isValidNodeCoord(nodeCoord));
		return nodeCoord.y * getSizeX() + nodeCoord.x;
	}

	GridBasedEnv::NodeCoord GridBasedEnv::toNodeCoord(int nodeID) const
	{
		FDK_ASSERT(isValidNodeID(nodeID));
		return NodeCoord(nodeID % getSizeX(), nodeID / getSizeX());
	}

	int GridBasedEnv::getNodeSpaceSize() const
	{
		return getSizeX() * getSizeY();
	}

	bool GridBasedEnv::isValidNodeCoord(const NodeCoord& nodeCoord) const
	{
		return nodeCoord.x >= 0 && nodeCoord.x < getSizeX()
			&& nodeCoord.y >= 0 && nodeCoord.y < getSizeY();
	}

	bool GridBasedEnv::isNodeWithCoordReachable(const NodeCoord& nodeCoord) const
	{
		return isNodeReachable(toNodeID(nodeCoord));
	}

	Navigator::Navigator()
		: m_environmentChecker(0)
	{
	}

	Navigator::~Navigator()
	{
	}
}}}
