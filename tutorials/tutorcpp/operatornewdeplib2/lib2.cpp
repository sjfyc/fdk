#include "lib2.h"
Lib2 lib2;

int* Lib2::newptr()
{
	return new int(100); // ���������û�obj�е�::operator new����������MSVCRT�е�::operator new
}
