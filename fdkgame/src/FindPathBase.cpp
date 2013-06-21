#include <fdkgame/FindPathBase.h>

namespace fdk { namespace game { namespace findpath
{
	bool Environment::isValidNodeID(int nodeID) const
	{
		return nodeID >= 0 && nodeID < getNodeSpaceSize();
	}

	PathFinder::PathFinder()
		: m_environmentChecker(0)
	{
	}

	PathFinder::~PathFinder()
	{
	}
}}}
