#ifndef __FDK_RAND_H_INCLUDE__
#define __FDK_RAND_H_INCLUDE__
#include <stdlib.h>
#include "Base.h"

namespace fdk
{
	// 需要先初始化随机数种子：srand(time(0));
	template <class T>
	inline T rand(const T& a, const T& b)
	{
		const T& minVal = minOf(a, b);
		const T& maxVal = maxOf(a, b);
		return static_cast<T>( ((double)::rand()/RAND_MAX)*(maxVal-minVal) + minVal );
	}
}

#endif
