#include "lib.h"

int* Lib::newptr()
{
	return new int(100); // ���������û�obj�е�::operator new����������MSVCRT�е�::operator new
}
