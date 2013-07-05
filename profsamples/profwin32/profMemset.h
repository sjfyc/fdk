#ifndef __PROFMEMSET_H
#define __PROFMEMSET_H
#include <math.h>
#include "util.h"

void profMemset()
{
	const int n = 1024*1024;
	int* a = new int[n];
	{UTIL_SCOPE_PROF
		memset(a, 0, sizeof(int)*n);
		std::cout << "memset cost ";
	}
	{UTIL_SCOPE_PROF
		for (size_t i = 0; i < n; ++i)
		{
			a[i] = 0;
		}
		std::cout << "for cost ";
	}
}
#endif

