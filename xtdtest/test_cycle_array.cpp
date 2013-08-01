#include <xtd/cycle_array.h>
#include <iostream>

template <class T>
void print(const xtd::cycle_array<T>& o)
{
	for (size_t i = 0; i < o.size(); ++i)
	{
		std::cout << "cycle_array[" << i << "] = " << o[i] << std::endl;
	}
}

void test_cycle_array()
{
	xtd::cycle_array<int> array(3);
	XTD_ASSERT(array.max_size() == 3);
	XTD_ASSERT(array.size() == 0);
	XTD_ASSERT(array.empty());
	array.push_back(1);
	array.push_back(2);
	XTD_ASSERT(array[0] == 1);
	XTD_ASSERT(array[1] == 2);
	array.push_back(3);
	array.push_back(4);
	array.push_back(5);
	XTD_ASSERT(array[0] == 3);
	XTD_ASSERT(array[1] == 4);
	XTD_ASSERT(array[2] == 5);
	array.push_front(6);
	XTD_ASSERT(array[0] == 6);
	XTD_ASSERT(array[1] == 3);
	XTD_ASSERT(array[2] == 4);
	array.push_front(7);
	XTD_ASSERT(array[0] == 7);
	XTD_ASSERT(array[1] == 6);
	XTD_ASSERT(array[2] == 3);
	array.pop_back();
	XTD_ASSERT(array.size() == 2);
	XTD_ASSERT(array[0] == 7);
	XTD_ASSERT(array[1] == 6);
	array.pop_front();
	XTD_ASSERT(array[0] == 6);
	array.clear();
	XTD_ASSERT(array.max_size() == 3);
	XTD_ASSERT(array.size() == 0);
	XTD_ASSERT(array.empty());
	array.push_back(1);
	array.push_back(2);
	array.push_back(3);
	array.push_back(4);
	XTD_ASSERT(array[0] == 2);
	XTD_ASSERT(array[1] == 3);
	XTD_ASSERT(array[2] == 4);
	array.adjust_max_size(5);
	array.push_back(5);
	array.push_back(6);
	XTD_ASSERT(array[3] == 5);
	XTD_ASSERT(array[4] == 6);
	XTD_ASSERT(array.max_size() == 5);
	XTD_ASSERT(array.size() == 5);
	XTD_ASSERT(!array.empty());
}
