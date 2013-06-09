#include "lib.h"
#include "operatornewdeplib2/lib2.h"

int* Lib::newptr()
{
	extern Lib2 lib2;
	lib2.create();
	lib2.destroy();
	return new int(100); // 优先链接用户obj中的::operator new，否则链接MSVCRT中的::operator new
}
