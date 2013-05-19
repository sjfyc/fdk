#ifndef __FDKGAME_BASE_H_INCLUDE__
#define __FDKGAME_BASE_H_INCLUDE__

/*
Build dll: Define FDKGAME_DLL FDKGAME_EXPORTS
Use dll: Define FDKGAME_DLL
Build/Use StaticLib: Nothing need to be defined
*/
#ifdef FDKGAME_DLL
	#ifdef FDKGAME_EXPORTS
	#define FDKGAME_API __declspec(dllexport)
	#else
	#define FDKGAME_API __declspec(dllimport)
	#endif
#else
	#define FDKGAME_API
#endif

#include <fdk/Base.h>
#include <utility> // std::swap

namespace fdk { namespace game {} }
namespace fdkgame=fdk::game;

namespace fdk { namespace game 
{
	template <class T>
	class Size
	{
	public:
		typedef T ValueType;
		Size();
		Size(T cx, T cy);
		void reset(T cx, T cy);
		bool operator==(const Size& other) const;
		bool operator<(const Size& other) const;
		FDK_AUTO_COMPARE_MEMBER(Size);
		T area() const;
		template <class U> U area() const;
		T cx;
		T cy;
	};

	template <class T>
	class Coord
	{
	public:
		typedef T ValueType;
		Coord();
		Coord(T x, T y);
		void reset(T x, T y);
		bool operator==(const Coord& other) const;
		bool operator<(const Coord& other) const;
		FDK_AUTO_COMPARE_MEMBER(Coord);
		template <class U> void offset(const Size<U>& size);
		template <class U> Coord offsetCopy(const Size<U>& size) const;
		T x;
		T y;
	};	

	template <class CoordT, class SizeT=CoordT>
	class Rect
	{
	public:
		typedef Coord<CoordT> CoordType;
		typedef Size<SizeT> SizeType;
		Rect();
		Rect(const CoordType& topLeft, const CoordType& bottomRight);
		Rect(const CoordType& topLeft, const SizeType& size);		
		void reset(const CoordType& topLeft, const CoordType& bottomRight);
		void reset(const CoordType& topLeft, const SizeType& size);
		bool isNormal() const;
		void normalize();
		Rect normalizeCopy() const;
		SizeT width() const;
		SizeT height() const;
		SizeType size() const;
		bool contain(const CoordType& coord) const;
		bool intersect(const Rect& other) const;
		bool intersect(const Rect& other, Rect& output) const;
		void offset(const SizeType& size);
		Rect offsetCopy(const SizeType& size) const;
		CoordType center() const;
		CoordType topLeft;
		CoordType bottomRight;
	};
	
	template <class T>
	inline Size<T>::Size() 
		: cx(), cy() 
	{}

	template <class T>
	inline Size<T>::Size(T _cx, T _cy) 
		: cx(_cx), cy(_cy) 
	{
	}

	template <class T>
	inline void Size<T>::reset(T _cx, T _cy) 
	{ 
		cx = _cx; 
		cy = _cy; 
	}

	template <class T>
	inline bool Size<T>::operator==(const Size& other) const
	{
		return cx == other.cx && cy == other.cy; 
	}

	template <class T>
	inline bool Size<T>::operator<(const Size& other) const 
	{
		if (cx != other.cx) 
		{
			return cx < other.cx;
		}
		return cy < other.cy;
	}

	template <class T>
	inline T Size<T>::area() const
	{
		return cx * cy;
	}

	template <class T>
	template <class U>
	inline U Size<T>::area() const
	{
		return static_cast<U>(cx) * static_cast<U>(cy);
	}

	template <class T>
	inline Coord<T>::Coord() 
		: x(), y() 
	{}

	template <class T>
	inline Coord<T>::Coord(T _x, T _y) 
		: x(_x), y(_y) 
	{}

	template <class T>
	inline void Coord<T>::reset(T _x, T _y) 
	{ 
		x = _x; 
		y = _y; 
	}

	template <class T>
	inline bool Coord<T>::operator==(const Coord& other) const
	{
		return x == other.x && y == other.y; 
	}

	template <class T>
	inline bool Coord<T>::operator<(const Coord& other) const 
	{
		if (x != other.x) 
		{ 
			return x < other.x; 
		}
		return y < other.y;
	}

	template <class T>
	template <class U>
	inline void Coord<T>::offset(const Size<U>& size)
	{
		x += size.cx;
		y += size.cy;
	}

	template <class T>
	template <class U>
	inline Coord<T> Coord<T>::offsetCopy(const Size<U>& size) const
	{
		return Coord(x+size.cx, y+size.cy);
	}

	template <class CoordT, class SizeT>
	inline Rect<CoordT, SizeT>::Rect()
		: topLeft(), bottomRight()
	{}

	template <class CoordT, class SizeT>
	inline Rect<CoordT, SizeT>::Rect(const CoordType& _topLeft, const CoordType& _bottomRight)
		: topLeft(_topLeft), bottomRight(_bottomRight)
	{}

	template <class CoordT, class SizeT>
	inline Rect<CoordT, SizeT>::Rect(const CoordType& _topLeft, const SizeType& size)
		: topLeft(_topLeft), bottomRight(_topLeft.offsetCopy(size))
	{}

	template <class CoordT, class SizeT>
	inline void Rect<CoordT, SizeT>::reset(const CoordType& _topLeft, const CoordType& _bottomRight)
	{
		topLeft = _topLeft;
		bottomRight = _bottomRight;
	}

	template <class CoordT, class SizeT>
	inline void Rect<CoordT, SizeT>::reset(const CoordType& _topLeft, const SizeType& size)
	{
		topLeft = _topLeft;
		bottomRight = _topLeft.offsetCopy(size);
	}
	
	template <class CoordT, class SizeT>
	inline bool Rect<CoordT, SizeT>::isNormal() const
	{
		return topLeft.x <= bottomRight.x && topLeft.y <= bottomRight.y;
	}

	template <class CoordT, class SizeT>
	inline void Rect<CoordT, SizeT>::normalize()
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

	template <class CoordT, class SizeT>
	inline Rect<CoordT, SizeT> Rect<CoordT, SizeT>::normalizeCopy() const
	{
		return Rect(
			CoordType(minOf(topLeft.x, bottomRight.x), minOf(topLeft.y, bottomRight.y)), 
			CoordType(maxOf(topLeft.x, bottomRight.x), maxOf(topLeft.y, bottomRight.y))
			);
	}

	template <class CoordT, class SizeT>
	inline SizeT Rect<CoordT, SizeT>::width() const
	{
		return static_cast<SizeT>(bottomRight.x - topLeft.x);
	}

	template <class CoordT, class SizeT>
	inline SizeT Rect<CoordT, SizeT>::height() const
	{
		return static_cast<SizeT>(bottomRight.y - topLeft.y);
	}

	template <class CoordT, class SizeT>
	inline typename Rect<CoordT, SizeT>::SizeType Rect<CoordT, SizeT>::size() const
	{
		return SizeType(width(), height());
	}

	template <class CoordT, class SizeT>
	inline bool Rect<CoordT, SizeT>::contain(const CoordType& point) const
	{
		FDK_ASSERT(isNormal());
		return point.x >= topLeft.x && point.x < bottomRight.x
			&& point.y >= topLeft.y && point.y < bottomRight.y;
	}

	template <class CoordT, class SizeT>
	inline bool Rect<CoordT, SizeT>::intersect(const Rect& other) const
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

	template <class CoordT, class SizeT>
	inline bool Rect<CoordT, SizeT>::intersect(const Rect& other, Rect& output) const
	{
		FDK_ASSERT(isNormal());
		FDK_ASSERT(other.isNormal());
		if (bottomRight.x <= other.topLeft.x || other.bottomRight.x <= topLeft.x ||
			bottomRight.y <= other.topLeft.y || other.bottomRight.y <= topLeft.y)
		{
			return false;
		}
		output.topLeft.x = maxOf(topLeft.x, other.topLeft.x);
		output.topLeft.y = maxOf(topLeft.y, other.topLeft.y);
		output.bottomRight.x = minOf(bottomRight.x, other.bottomRight.x);
		output.bottomRight.y = minOf(bottomRight.y, other.bottomRight.y);
		return true;
	}

	template <class CoordT, class SizeT>
	inline void Rect<CoordT, SizeT>::offset(const SizeType& size)
	{
		topLeft.offset(size);
		bottomRight.offset(size);
	}

	template <class CoordT, class SizeT>
	inline Rect<CoordT, SizeT> Rect<CoordT, SizeT>::offsetCopy(const SizeType& size) const
	{
		return Rect(topLeft.offsetCopy(size), bottomRight.offsetCopy(size));
	}

	template <class CoordT, class SizeT>
	inline typename Rect<CoordT, SizeT>::CoordType Rect<CoordT, SizeT>::center() const
	{
		return CoordType((topLeft.x+bottomRight.x)/2, (topLeft.y+bottomRight.y)/2);
	}

	typedef Coord<short> CellCoord;
	typedef Size<short> CellSize;
	typedef Size<short> CellOffset;
	typedef Rect<short> CellRange;
	typedef Coord<float> Location;
	typedef Size<float> LocationOffset;

	inline CellCoord locationToCellCoord(const Location& location, const CellSize& cellSize)
	{
		return CellCoord(
			static_cast<CellCoord::ValueType>(location.x/cellSize.cx),
			static_cast<CellCoord::ValueType>(location.y/cellSize.cy)
			);
	}

	inline Location cellCoordToLocation(const CellCoord& cellCoord, const CellSize& cellSize)
	{
		return Location(
			static_cast<Location::ValueType>(cellCoord.x) * cellSize.cx,
			static_cast<Location::ValueType>(cellCoord.y) * cellSize.cy
			);
	}
}}

#endif
