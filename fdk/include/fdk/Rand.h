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
		return (::rand()/RAND_MAX+1.0)*(maxVal-minVal+1) + minVal;
	}
}

#endif
