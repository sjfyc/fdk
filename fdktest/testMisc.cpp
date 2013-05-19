#include <fdk/Misc.h>
#include <string.h>

void testMisc(const char* argv0)
{
	FDK_ASSERT(strcmp(fdk::getFileName(argv0), "fdktest.exe") == 0);
	FDK_ASSERT(strcmp(fdk::getFileName(""), "") == 0);
	FDK_ASSERT(strcmp(fdk::getFileName("ab"), "ab") == 0);
	FDK_ASSERT(strcmp(fdk::getFileName("\\ab"), "ab") == 0);
	FDK_ASSERT(strcmp(fdk::getFileName("ab\\"), "") == 0);
	FDK_ASSERT(strcmp(fdk::getFileName("ab\\cd"), "cd") == 0);
}
