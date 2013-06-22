#ifndef __FDKGAME_NAVIVERTEXMAP_H_INCLUDE__
#define __FDKGAME_NAVIVERTEXMAP_H_INCLUDE__
#include <fdk/Array2D.h>
#include "NaviTypes.h"

namespace fdk { namespace game { namespace navi
{
	class BlockMap;

	typedef Vector2D<int> VertexCoord;

	// 对应某种尺寸单位的顶点地图：每个顶点存储对于该尺寸单位的碰撞数量
	class FDKGAME_API VertexMap
	{
	public:
		VertexMap(int unitSize);
		~VertexMap();
		void rebuildFromBlockMap(const BlockMap& blockMap);
		void onSetBlock(const CellCoord& cellCoord, bool bSet);
	private:
		int m_unitSize;
		Array2D<int> m_data;
	};
}}}

#endif
