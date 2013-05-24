#include <fdk/String.h>

void testString()
{
	std::string orign = "  abc  ";
	std::string s;

	s = orign;
	fdk::trimLeft(s);
	FDK_ASSERT(s == "abc  ");

	s = orign;
	fdk::trimRight(s);
	FDK_ASSERT(s == "  abc");

	s = orign;
	fdk::trim(s);
	FDK_ASSERT(s == "abc");

	orign = "     ";
	
	s = orign;
	fdk::trimLeft(s);
	FDK_ASSERT(s == "");

	s = orign;
	fdk::trimRight(s);
	FDK_ASSERT(s == "");

	s = orign;
	fdk::trim(s);
	FDK_ASSERT(s == "");

	s = " ab  12  abc 12 ";
	fdk::replace(s, "ab", "c");
	FDK_ASSERT(s == " c  12  cc 12 ");
}
