#include <fdkgame/Math.h>

void testMath()
{
	using namespace fdkgame;
	typedef Vector2D<int> IVector2D;
	typedef Rect<int> IRect;

	IVector2D v;
	FDK_ASSERT(v.x == 0 && v.y == 0);
	FDK_ASSERT(v == IVector2D::ZERO);	
	IVector2D vx(v);
	FDK_ASSERT(vx == v);
	v.reset(1, 2);
	IVector2D vx2(v);
	FDK_ASSERT(vx2.x == 1 && vx2.y == 2);
	FDK_ASSERT(-v == IVector2D(-1, -2));
	v.reset();
	FDK_ASSERT(v == IVector2D::ZERO);
	v.reset(3, 5);
	v += IVector2D(3, 4);
	FDK_ASSERT(v == IVector2D(6, 9));
	v -= IVector2D(2, 2);
	FDK_ASSERT(v == IVector2D(4, 7));
	v *= 3;
	FDK_ASSERT(v == IVector2D(12, 21));
	v /= 0.5f;
	FDK_ASSERT(v == IVector2D(24, 42));
	FDK_ASSERT(v+IVector2D(1,2)==IVector2D(25,44));
	FDK_ASSERT(v-IVector2D(1,2)==IVector2D(23,40));
	FDK_ASSERT(v*0.5==IVector2D(12,21));
	FDK_ASSERT(v/3==IVector2D(8,14));

	IRect rect;
	FDK_ASSERT(rect.topLeft == IVector2D::ZERO);
	FDK_ASSERT(rect.bottomRight == IVector2D::ZERO);
	FDK_ASSERT(rect.size() == IVector2D::ZERO);
	FDK_ASSERT(!rect.contain(IVector2D::ZERO));
	FDK_ASSERT(rect.isNormal());
	rect.reset(IVector2D(1, 2), IVector2D(4, 4));
	FDK_ASSERT(rect.topLeft == IVector2D(1, 2));
	FDK_ASSERT(rect.bottomRight == IVector2D(4, 4));
	FDK_ASSERT(rect.width() == 3);
	FDK_ASSERT(rect.height() == 2);
	FDK_ASSERT(rect.size() == IVector2D(3, 2));

	IRect rect2(1, 2, 3, 4);
	FDK_ASSERT(rect2.contain(IVector2D(1, 2)));
}
