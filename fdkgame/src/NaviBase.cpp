#include <fdkgame/NaviBase.h>

namespace fdk { namespace game { namespace navi
{
	const GridBasedEnv* Environment::toGridBaseEnv() const
	{
		return 0;
	}

	GridBasedEnv* Environment::toGridBaseEnv()
	{
		return 0;
	}

	int GridBasedEnv::getNodeSpaceSize() const
	{
		return getSizeX() * getSizeY();
	}
	
	const GridBasedEnv* GridBasedEnv::toGridBaseEnv() const
	{
		return this;
	}

	GridBasedEnv* GridBasedEnv::toGridBaseEnv()
	{
		return this;
	}	
}}}
