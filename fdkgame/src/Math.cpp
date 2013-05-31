#include <fdkgame/Math.h>

namespace fdk { namespace game 
{
	FDKGAME_API float invSqrt(float x)
	{// from hge
		union
		{
			int intPart;
			float floatPart;
		} convertor;

		convertor.floatPart = x;
		convertor.intPart = 0x5f3759df - (convertor.intPart >> 1);
		return convertor.floatPart*(1.5f - 0.4999f*x*convertor.floatPart*convertor.floatPart);
	}
}}