#ifndef __FDK_ARRAY2D_H_INCLUDE__
#define __FDK_ARRAY2D_H_INCLUDE__
#include "Base.h"
#include <memory>
#include "Memory.h"

namespace fdk
{
	template <class T, class AllocatorT=std::allocator<T> >
	class Array2D
	{
	public:
		typedef AllocatorT allocator_type;
		typedef typename AllocatorT::size_type size_type;
		typedef T value_type;
		Array2D(size_type sizeX, size_type sizeY);
		Array2D(size_type sizeX, size_type sizeY, const T& value);
		Array2D(const Array2D& other);
		~Array2D();		
		T& operator()(size_type x, size_type y);
		const T& operator()(size_type x, size_type y) const;
		size_type size_x() const;
		size_type size_y() const;
	private:
		AllocatorT m_allocator;
		size_type m_sizeX;
		size_type m_sizeY;
		T* m_data;
	};

	template <class T, class AllocatorT>
	Array2D<T, AllocatorT>::Array2D(size_type sizeX, size_type sizeY)
		: m_allocator()
		, m_sizeX(sizeX)
		, m_sizeY(sizeY)
		, m_data(newDefaultByAllocator(sizeX*sizeY, m_allocator))
	{
		FDK_ASSERT(m_sizeY > 0 && m_sizeY > 0);
	}

	template <class T, class AllocatorT>
	Array2D<T, AllocatorT>::Array2D(size_type sizeX, size_type sizeY, const T& value)
		: m_allocator()
		, m_sizeX(sizeX)
		, m_sizeY(sizeY)
		, m_data(newFillByAllocator(sizeX*sizeY, value, m_allocator))
	{
		FDK_ASSERT(m_sizeY > 0 && m_sizeY > 0);
	}

	template <class T, class AllocatorT>
	Array2D<T, AllocatorT>::Array2D(const Array2D& other)
		: m_allocator()
		, m_sizeX(other.m_sizeX)
		, m_sizeY(other.m_sizeY)
		, m_data(newCopyNByAllocator(other.m_data, m_sizeX*m_sizeY, m_allocator))
	{
	}

	template <class T, class AllocatorT>
	Array2D<T, AllocatorT>::~Array2D()
	{	
		deleteNByAllocator(m_data, m_sizeX*m_sizeY, m_allocator);
	}
	
	template <class T, class AllocatorT>
	inline T& Array2D<T, AllocatorT>::operator()(size_type x, size_type y)
	{
		return const_cast<T&>(
			static_cast<const Array2D<T>&>(*this)(x, y)
			);
	}

	template <class T, class AllocatorT>
	inline const T& Array2D<T, AllocatorT>::operator()(size_type x, size_type y) const
	{
		FDK_ASSERT(x >= 0 && x < m_sizeX);
		FDK_ASSERT(y >= 0 && y < m_sizeY);
		return m_data[y*m_sizeX+x];
	}

	template <class T, class AllocatorT>
	inline typename Array2D<T, AllocatorT>::size_type Array2D<T, AllocatorT>::size_x() const
	{
		return m_sizeX;
	}

	template <class T, class AllocatorT>
	inline typename Array2D<T, AllocatorT>::size_type Array2D<T, AllocatorT>::size_y() const
	{
		return m_sizeY;
	}
}

#endif
