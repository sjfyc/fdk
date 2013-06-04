#include <math.h>
#include "util.h"

float invSqrt_hge(float x)
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

float invSqrt_unreal3( float F )
{
#if ENABLE_VECTORINTRINSICS
	static const __m128 fThree = _mm_set_ss( 3.0f );
	static const __m128 fOneHalf = _mm_set_ss( 0.5f );
	__m128 Y0, X0, Temp;
	float temp;

	Y0 = _mm_set_ss( F );
	X0 = _mm_rsqrt_ss( Y0 );	// 1/sqrt estimate (12 bits)

	// Newton-Raphson iteration (X1 = 0.5*X0*(3-(Y*X0)*X0))
	Temp = _mm_mul_ss( _mm_mul_ss(Y0, X0), X0 );	// (Y*X0)*X0
	Temp = _mm_sub_ss( fThree, Temp );				// (3-(Y*X0)*X0)
	Temp = _mm_mul_ss( X0, Temp );					// X0*(3-(Y*X0)*X0)
	Temp = _mm_mul_ss( fOneHalf, Temp );			// 0.5*X0*(3-(Y*X0)*X0)
	_mm_store_ss( &temp, Temp );

	return temp;

#elif _WIN64
	// NOTE: This is only really used when included from C++/CLI source files in 64-bit
	// @todo win64: Use a more efficient implementation here!
	return 1.0f / sqrtf( F );
#else
	// x86 ASM
	const float fThree = 3.0f;
	const float fOneHalf = 0.5f;
	float temp;

	__asm
	{
		movss	xmm1,[F]
		rsqrtss	xmm0,xmm1			// 1/sqrt estimate (12 bits)

		// Newton-Raphson iteration (X1 = 0.5*X0*(3-(Y*X0)*X0))
		movss	xmm3,[fThree]
		movss	xmm2,xmm0
		mulss	xmm0,xmm1			// Y*X0
		mulss	xmm0,xmm2			// Y*X0*X0
		mulss	xmm2,[fOneHalf]		// 0.5*X0
		subss	xmm3,xmm0			// 3-Y*X0*X0
		mulss	xmm3,xmm2			// 0.5*X0*(3-Y*X0*X0)
		movss	[temp],xmm3
	}
	return temp;
#endif
}

void profSqrt()
{
	const size_t nSamples = 10000000;
	float addon = 37.33f;	
	{UTIL_SCOPE_PROF
		for (size_t i = 0; i < nSamples; ++i)
		{
			float f = 1.f/::sqrtf(nSamples+addon);
		}
		std::cout << "1.f/::sqrtf ";
	}

	{UTIL_SCOPE_PROF
		for (size_t i = 0; i < nSamples; ++i)
		{
			float f = invSqrt_hge(nSamples+addon);
		}
		std::cout << "invSqrt_hge ";
	}

	{UTIL_SCOPE_PROF
		for (size_t i = 0; i < nSamples; ++i)
		{
			float f = invSqrt_unreal3(nSamples+addon);
		}
		std::cout << "invSqrt_unreal3 ";
	}

	float fixValue = 32385.337f;
	std::cout << "1.f/::sqrtf(" << fixValue << ")=" << 1.f/::sqrtf(fixValue) << std::endl;
	std::cout << "invSqrt_hge(" << fixValue << ")=" << invSqrt_hge(fixValue) << std::endl;
	std::cout << "invSqrt_unreal3(" << fixValue << ")=" << invSqrt_unreal3(fixValue) << std::endl;
}
