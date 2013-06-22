#ifndef __FDKGAME_NAVIBLOCKMAP_H_INCLUDE__
#define __FDKGAME_NAVIBLOCKMAP_H_INCLUDE__
#include <fdk/Array2D.h>
#include "NaviTypes.h"

namespace fdk { namespace game { namespace navi
{
	class TileMap;

	// ��Ӧĳһ���ƶ��������ϰ���ͼ��ÿ������ֻ�洢�Ƿ�Ϊ�ϰ�
	class FDKGAME_API BlockMap
	{
		friend class MapManager;
	public:
		BlockMap(TileMap& tileMap, MoveCapability moveCapability);
		~BlockMap();
		void rebuildFromTileMap();		
		size_t getSizeX() const;
		size_t getSizeY() const;
		MoveCapability getMoveCapability() const;
		bool isBlock(const CellCoord& cellCoord) const;
	private:
		TileMap& m_tileMap;
		MoveCapability m_moveCapability;
		Array2D<bool> m_data;
	};
}}}

#endif
