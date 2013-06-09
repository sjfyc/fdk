#ifndef __LIB_H
#define __LIB_H
#include <iostream>

class Lib
{
public:
	Lib() : m_value(0) {}
	void create()
	{
		std::cout << "Lib::create" << std::endl;
		m_value = new int(10);
	}
	void destroy()
	{
		std::cout << "Lib::destroy" << std::endl;
		delete m_value;
		m_value = 0;
	}
	int* newptr();
private:
	int* m_value;
};

#endif
