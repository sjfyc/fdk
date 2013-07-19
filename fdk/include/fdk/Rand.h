#ifndef __FDK_RAND_H_INCLUDE__
#define __FDK_RAND_H_INCLUDE__
#include "Base.h"
#include <stdlib.h>
#include <time.h>

namespace fdk
{
	// 需要先初始化随机数种子：srand(time(0));
	//template <class T>
	//inline T rand(const T& a, const T& b)
	//{
	//	const T& minVal = minOf(a, b);
	//	const T& maxVal = maxOf(a, b);
	//	return static_cast<T>( ((double)::rand()/RAND_MAX)*(maxVal-minVal) + minVal );
	//}

	void initRand()
	{
		::srand((unsigned int)(time(0)));
	}

	// [a,b]
	int rand(int a, int b)
	{
		int minValue = a < b ? a : b;
		int maxValue = a > b ? a : b;
		return (::rand() % (maxValue-minValue+1))+ minValue;
	}

	// [0,1]
	double rand01()
	{
		return ::rand() / double(RAND_MAX);
	}
}

#endif
