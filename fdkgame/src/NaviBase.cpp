#include <fdkgame/NaviBase.h>

namespace fdk { namespace game { namespace navi
{
	bool Environment::isValidNodeID(int nodeID) const
	{
		return nodeID >= 0 && nodeID < getNodeSpaceSize();
	}

	Navigator::Navigator()
		: m_environmentChecker(0)
	{
	}

	Navigator::~Navigator()
	{
	}
}}}
