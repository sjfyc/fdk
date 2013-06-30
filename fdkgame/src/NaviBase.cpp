#include <fdkgame/NaviBase.h>

namespace fdk { namespace game { namespace navi
{
	const GridEnv* Environment::toGridEnv() const
	{
		return 0;
	}

	GridEnv* Environment::toGridEnv()
	{
		return 0;
	}

	int GridEnv::getNodeSpaceSize() const
	{
		return getSizeX() * getSizeY();
	}
	
	const GridEnv* GridEnv::toGridEnv() const
	{
		return this;
	}

	GridEnv* GridEnv::toGridEnv()
	{
		return this;
	}	
}}}
