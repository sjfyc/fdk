#include <fdkgame/FindPathBase.h>

namespace fdk { namespace game { namespace findpath
{
	int PathFinder::getMinClearanceValueRequired() const
	{
		return 1;
	}

	bool Environment::isValidNodeID(int nodeID) const
	{
		return nodeID >= 0 && nodeID < getNodeSpaceSize();
	}
}}}
