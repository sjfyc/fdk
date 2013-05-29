#include <fdk/Array2D.h>

void testArray2D()
{
	fdk::Array2D<int> ia(2, 3);
	for (size_t y = 0; y < 3; ++y)
	{
		for (size_t x = 0; x < 2; ++x)
		{
			FDK_ASSERT(ia(x, y) == 0);
			ia(x, y) = x+y;
		}
	}
	
	fdk::Array2D<int> ia1(2, 2, 5);
	FDK_ASSERT(ia1(0, 0) == 5);
	FDK_ASSERT(ia1(0, 1) == 5);
	FDK_ASSERT(ia1(1, 0) == 5);
	FDK_ASSERT(ia1(1, 1) == 5);
	ia1(1, 1) = 3;
	FDK_ASSERT(ia1(1, 1) == 3);

	fdk::Array2D<int> ia2(ia);
	for (size_t y = 0; y < ia2.size_y(); ++y)
	{
		for (size_t x = 0; x < ia2.size_x(); ++x)
		{
			FDK_ASSERT(ia2(x, y) == x+y);
		}
	}
}
