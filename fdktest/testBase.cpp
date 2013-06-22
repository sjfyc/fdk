#include <fdk/Misc.h>
#include <string.h>

namespace TestBase
{
	enum 
	{
		E_0,
		E_1,
		E_2
	};
}

void testBase()
{
	{
		int maskValue = 0;
		FDK_ASSERT(!fdk::hasEnumMask(maskValue, TestBase::E_0));
		fdk::setEnumMask(maskValue, TestBase::E_0);
		FDK_ASSERT(fdk::hasEnumMask(maskValue, TestBase::E_0));
		fdk::clearEnumMask(maskValue, TestBase::E_0);
		FDK_ASSERT(!fdk::hasEnumMask(maskValue, TestBase::E_0));
		fdk::setEnumMask(maskValue, TestBase::E_0);
		fdk::setEnumMask(maskValue, TestBase::E_1);
		FDK_ASSERT(fdk::hasEnumMask(maskValue, TestBase::E_1));
	}
}
