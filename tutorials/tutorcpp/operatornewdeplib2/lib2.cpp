#include "lib2.h"
Lib2 lib2;

int* Lib2::newptr()
{
	return new int(100); // 优先链接用户obj中的::operator new，否则链接MSVCRT中的::operator new
}
