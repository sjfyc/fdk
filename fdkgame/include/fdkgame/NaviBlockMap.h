#ifndef __FDKGAME_NAVIBLOCKMAP_H_INCLUDE__
#define __FDKGAME_NAVIBLOCKMAP_H_INCLUDE__
#include <fdk/Array2D.h>
#include "NaviTypes.h"

namespace fdk { namespace game { namespace navi
{
	class TileMap;

	// 对应某一种移动能力的障碍地图：每个格子只存储是否为障碍
	class FDKGAME_API BlockMap
	{
	public:
		BlockMap(int moveCapability);
		~BlockMap();
		void rebuildFromTileMap(const TileMap& tileMap);		
	private:
		int m_moveCapability;
		Array2D<bool> m_data;
	};
}}}

#endif
