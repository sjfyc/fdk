#include "lib2.h"

int* Lib2::newptr()
{
	return new int(100); // ���������û�obj�е�::operator new����������MSVCRT�е�::operator new
}
