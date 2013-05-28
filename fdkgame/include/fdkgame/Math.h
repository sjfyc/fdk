#ifndef __FDKGAME_MATH_H_INCLUDE__
#define __FDKGAME_MATH_H_INCLUDE__
#include "Base.h"

namespace fdk { namespace game 
{
	template <class T>
	class Vector2D
	{
	public:
		typedef T ValueType;
		static Vector2D ZERO;
		Vector2D();
		Vector2D(const Vector2D& other);
		Vector2D(T x, T y);
		void reset(T x, T y);
		Vector2D operator+(const Vector2D& other) const;
		Vector2D operator-(const Vector2D& other) const;
		Vector2D operator-() const;
		Vector2D& operator+=(const Vector2D& other);
		Vector2D& operator-=(const Vector2D& other);
		T x;
		T y;
	};

	template <class T>
	Vector2D<T> Vector2D<T>::ZERO();

	template <class T>
	inline Vector2D<T>::Vector2D()
		: x(), y()
	{}
	
	template <class T>
	inline Vector2D<T>::Vector2D(const Vector2D& other)
		: x(other.x), y(other.y)
	{}

	template <class T>
	inline Vector2D<T>::Vector2D(T _x, T _y)
		: x(_x), y(_y)
	{}

	template <class T>
	inline void Vector2D<T>::reset(T _x, T _y)
	{
		x = _x;
		y = _y;
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
	inline Vector2D<T> Vector2D<T>::operator-() const
	{
		return Vector2D(-x, -y);
	}

	template <class T>
	Vector2D<T>& Vector2D<T>::operator+=(const Vector2D& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	
	template <class T>
	Vector2D<T>& Vector2D<T>::operator-=(const Vector2D& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}
}}

#endif
