#include "lib.h"
#include "operatornewdeplib2/lib2.h"

int* Lib::newptr()
{
	extern Lib2 lib2;
	lib2.create();
	lib2.destroy();
	return new int(100); // ���������û�obj�е�::operator new����������MSVCRT�е�::operator new
}
