#ifndef __FDKGAME_MATH_H_INCLUDE__
#define __FDKGAME_MATH_H_INCLUDE__
#include "Base.h"
#include <utility>

namespace fdk { namespace game 
{
	template <class T>
	class Vector2D
	{
	public:
		typedef T ValueType;
		static const Vector2D ZERO;
		Vector2D();
		Vector2D(T x, T y);
		void reset();
		void reset(T x, T y);
		bool operator==(const Vector2D& other) const;
		bool operator<(const Vector2D& other) const;
		FDK_AUTO_COMPARE_MEMBER(Vector2D)
		Vector2D operator-() const;
		Vector2D operator+(const Vector2D& other) const;
		Vector2D operator-(const Vector2D& other) const;
		Vector2D operator*(float scale) const;
		Vector2D operator/(float scale) const;
		Vector2D& operator+=(const Vector2D& other);
		Vector2D& operator-=(const Vector2D& other);
		Vector2D& operator*=(float scale);
		Vector2D& operator/=(float scale);
		T x;
		T y;
	};

	template <class T>
	class Rect
	{
	public:
		typedef T ValueType;
		typedef Vector2D<T> VectorType;
		static Rect makeRectFromCenter(const VectorType& center, const VectorType& extent);
		Rect();
		Rect(const VectorType& topLeft, const VectorType& bottomRight);
		Rect(T x0, T y0, T x1, T y1);
		void reset();
		void reset(const VectorType& topLeft, const VectorType& bottomRight);
		void reset(T x0, T y0, T x1, T y1);
		void resetFromCenter(const VectorType& center, const VectorType& extent);
		void normalize();
		bool isNormal() const;		
		bool contain(const VectorType& in) const;
		void clip(const Rect& other);
		bool intersect(const Rect& other) const;
		bool intersect(const Rect& other, Rect& output) const;
		VectorType size() const;
		T width() const;
		T height() const;
		VectorType center() const;
		VectorType extent() const;
		VectorType topLeft;
		VectorType bottomRight;
	};

	template <class T>
	const Vector2D<T> Vector2D<T>::ZERO;

	template <class T>
	inline Vector2D<T>::Vector2D()
		: x(), y()
	{}
	
	template <class T>
	inline Vector2D<T>::Vector2D(T _x, T _y)
		: x(_x), y(_y)
	{}

	template <class T>
	inline void Vector2D<T>::reset()
	{
		x = T();
		y = T();
	}

	template <class T>
	inline void Vector2D<T>::reset(T _x, T _y)
	{
		x = _x;
		y = _y;
	}
	
	template <class T>
	inline bool Vector2D<T>::operator==(const Vector2D& other) const
	{
		return x == other.x && y == other.y;
	}

	template <class T>
	inline bool Vector2D<T>::operator<(const Vector2D& other) const
	{
		if (x != other.x)
		{
			return x < other.x;
		}
		return y < other.y;
	}

	template <class T>
	inline Vector2D<T> Vector2D<T>::operator-() const
	{
		return Vector2D(-x, -y);
	}

	template <class T>
	inline Vector2D<T> Vector2D<T>::operator+(const Vector2D& other) const
	{
		return Vector2D(x+other.x, y+other.y);
	}

	template <class T>
	inline Vector2D<T> Vector2D<T>::operator-(const Vector2D& other) const
	{
		return Vector2D(x-other.x, y-other.y);
	}
	
	template <class T>
	inline Vector2D<T> Vector2D<T>::operator*(float scale) const
	{
		return Vector2D(
			static_cast<T>(x*scale),
			static_cast<T>(y*scale)
			);
	}

	template <class T>
	inline Vector2D<T> Vector2D<T>::operator/(float scale) const
	{
		FDK_ASSERT(scale != 0);
		return Vector2D(
			static_cast<T>(x/scale),
			static_cast<T>(y/scale)
			);
	}

	template <class T>
	inline Vector2D<T>& Vector2D<T>::operator+=(const Vector2D& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	
	template <class T>
	inline Vector2D<T>& Vector2D<T>::operator-=(const Vector2D& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	template <class T>
	inline Vector2D<T>& Vector2D<T>::operator*=(float scale)
	{
		x = static_cast<T>(x * scale);
		y = static_cast<T>(y * scale);
		return *this;
	}

	template <class T>
	inline Vector2D<T>& Vector2D<T>::operator/=(float scale)
	{
		FDK_ASSERT(scale != 0);
		x = static_cast<T>(x / scale);
		y = static_cast<T>(y / scale);
		return *this;
	}

	template <class T>
	inline Rect<T> Rect<T>::makeRectFromCenter(const VectorType& center, const VectorType& extent)
	{
		return Rect(center-extent, center+extent);
	}

	template <class T>
	inline Rect<T>::Rect()
		: topLeft()
		, bottomRight()
	{}

	template <class T>
	inline Rect<T>::Rect(const VectorType& _topLeft, const VectorType& _bottomRight)
		: topLeft(_topLeft)
		, bottomRight(_bottomRight)
	{}

	template <class T>
	inline Rect<T>::Rect(T x0, T y0, T x1, T y1)
		: topLeft(x0, y0)
		, bottomRight(x1, y1)
	{}

	template <class T>
	inline void Rect<T>::reset()
	{
		topLeft.reset();
		bottomRight.reset();
	}

	template <class T>
	inline void Rect<T>::reset(const VectorType& _topLeft, const VectorType& _bottomRight)
	{
		topLeft = _topLeft;
		bottomRight = _bottomRight;
	}

	template <class T>
	inline void Rect<T>::reset(T x0, T y0, T x1, T y1)
	{
		topLeft.reset(x0, y0);
		bottomRight.reset(x1, y1);
	}

	template <class T>
	inline void Rect<T>::resetFromCenter(const VectorType& center, const VectorType& extent)
	{
		topLeft = center-extent;
		bottomRight = center+extent;
	}

	template <class T>
	inline void Rect<T>::normalize()
	{
		if (topLeft.x > bottomRight.x)
		{
			std::swap(topLeft.x, bottomRight.x);
		}
		if (topLeft.y > bottomRight.y)
		{
			std::swap(topLeft.y, bottomRight.y);
		}
	}

	template <class T>
	inline bool Rect<T>::isNormal() const
	{
		return topLeft.x <= bottomRight.x && topLeft.y <= bottomRight.y;
	}

	template <class T>
	inline bool Rect<T>::contain(const VectorType& in) const
	{
		FDK_ASSERT(isNormal());
		return in.x >= topLeft.x && in.x < bottomRight.x
			&& in.y >= topLeft.y && in.y < bottomRight.y;
	}

	template <class T>
	inline void Rect<T>::clip(const Rect& other)
	{
		FDK_ASSERT(isNormal());
		FDK_ASSERT(other.isNormal());
		topLeft.x = maxOf(topLeft.x, other.topLeft.x);
		topLeft.y = maxOf(topLeft.y, other.topLeft.y);
		bottomRight.x = minOf(bottomRight.x, other.bottomRight.x);
		bottomRight.y = minOf(bottomRight.y, other.bottomRight.y);
		bottomRight.x = maxOf(topLeft.x, bottomRight.x);
		bottomRight.y = maxOf(topLeft.y, bottomRight.y);
	}

	template <class T>
	inline bool Rect<T>::intersect(const Rect& other) const
	{
		FDK_ASSERT(isNormal());
		FDK_ASSERT(other.isNormal());
		if (bottomRight.x <= other.topLeft.x || other.bottomRight.x <= topLeft.x ||
			bottomRight.y <= other.topLeft.y || other.bottomRight.y <= topLeft.y)
		{
			return false;
		}
		return true;
	}

	template <class T>
	inline bool Rect<T>::intersect(const Rect& other, Rect& output) const
	{
		FDK_ASSERT(isNormal());
		FDK_ASSERT(other.isNormal());
		if (bottomRight.x <= other.topLeft.x || other.bottomRight.x <= topLeft.x ||
			bottomRight.y <= other.topLeft.y || other.bottomRight.y <= topLeft.y)
		{
			return false;
		}
		output.reset(
			maxOf(topLeft.x, other.topLeft.x),
			maxOf(topLeft.y, other.topLeft.y),
			minOf(bottomRight.x, other.bottomRight.x),
			minOf(bottomRight.y, other.bottomRight.y)
			);
		return true;
	}

	template <class T>
	inline typename Rect<T>::VectorType Rect<T>::size() const
	{
		return bottomRight-topLeft;
	}

	template <class T>
	inline T Rect<T>::width() const
	{
		return bottomRight.x-topLeft.x;
	}

	template <class T>
	inline T Rect<T>::height() const
	{
		return bottomRight.y-topLeft.y;
	}

	template <class T>
	inline typename Rect<T>::VectorType Rect<T>::center() const
	{
		return (topLeft+bottomRight)*0.5f;
	}

	template <class T>
	inline typename Rect<T>::VectorType Rect<T>::extent() const
	{
		return (bottomRight-topLeft)*0.5f;
	}
}}

#endif
