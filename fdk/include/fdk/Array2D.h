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
		Array2D();
		Array2D(size_type sizeX, size_type sizeY);
		Array2D(size_type sizeX, size_type sizeY, const T& value);
		Array2D(const Array2D& other);
		~Array2D();
		void clear();
		void reset(size_type sizeX, size_type sizeY);
		void reset(size_type sizeX, size_type sizeY, const T& value);
		Array2D& operator=(const Array2D& other);
		T& operator()(size_type x, size_type y);
		const T& operator()(size_type x, size_type y) const;
		bool empty() const;
		size_type count() const;
		size_type size_x() const;
		size_type size_y() const;
		T* raw_data() const;
		bool is_valid_index(size_type x, size_type y) const;
		bool is_valid_index(size_type index) const;
		size_type to_index(size_type x, size_type y) const;
		void to_index(size_t index, size_type& x, size_type& y) const;
	private:
		AllocatorT m_allocator;
		size_type m_sizeX;
		size_type m_sizeY;
		T* m_data;
	};

	template <class T, class AllocatorT>
	Array2D<T, AllocatorT>::Array2D()
		: m_allocator()
		, m_sizeX(0)
		, m_sizeY(0)
		, m_data(0)
	{
	}

	template <class T, class AllocatorT>
	Array2D<T, AllocatorT>::Array2D(size_type sizeX, size_type sizeY)
		: m_allocator()
		, m_sizeX(sizeX)
		, m_sizeY(sizeY)
		, m_data(newDefaultByAllocator(sizeX*sizeY, m_allocator))
	{
		FDK_ASSERT(m_sizeX > 0 && m_sizeY > 0);
	}

	template <class T, class AllocatorT>
	Array2D<T, AllocatorT>::Array2D(size_type sizeX, size_type sizeY, const T& value)
		: m_allocator()
		, m_sizeX(sizeX)
		, m_sizeY(sizeY)
		, m_data(newFillByAllocator(sizeX*sizeY, value, m_allocator))
	{
		FDK_ASSERT(m_sizeX > 0 && m_sizeY > 0);
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
		clear();
	}
	
	template <class T, class AllocatorT>
	void Array2D<T, AllocatorT>::clear()
	{
		if (m_data)
		{
			deleteNByAllocator(m_data, m_sizeX*m_sizeY, m_allocator);
			m_sizeX = 0;
			m_sizeY = 0;
			m_data = 0;
		}
		FDK_ASSERT(m_sizeX == 0);
		FDK_ASSERT(m_sizeY == 0);
		FDK_ASSERT(!m_data);
	}

	template <class T, class AllocatorT>
	void Array2D<T, AllocatorT>::reset(size_type sizeX, size_type sizeY)
	{
		FDK_ASSERT(sizeX > 0 && sizeY > 0);
		try
		{
			T* data = newDefaultByAllocator(sizeX*sizeY, m_allocator);
			clear();
			m_sizeX = sizeX;
			m_sizeY = sizeY;
			m_data = data;
		}		
		catch (...)
		{
			throw;
		}
	}

	template <class T, class AllocatorT>
	void Array2D<T, AllocatorT>::reset(size_type sizeX, size_type sizeY, const T& value)
	{
		FDK_ASSERT(sizeX > 0 && sizeY > 0);
		try
		{
			T* data = newFillByAllocator(sizeX*sizeY, value, m_allocator);
			clear();
			m_sizeX = sizeX;
			m_sizeY = sizeY;
			m_data = data;
		}		
		catch (...)
		{
			throw;
		}
	}

	template <class T, class AllocatorT>
	Array2D<T, AllocatorT>& Array2D<T, AllocatorT>::operator=(const Array2D& other)
	{
		if (this == &other)
		{
			return *this;
		}
		try
		{
			T* data = newCopyNByAllocator(other.m_data, other.m_sizeX*other.m_sizeY, m_allocator);
			clear();
			m_sizeX = other.m_sizeX;
			m_sizeY = other.m_sizeY;
			m_data = data;
		}		
		catch (...)
		{
			throw;
		}
		return *this;
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
		FDK_ASSERT(is_valid_index(x, y));
		return m_data[y*m_sizeX+x];
	}

	template <class T, class AllocatorT>
	inline bool Array2D<T, AllocatorT>::empty() const
	{
		return !m_data;
	}

	template <class T, class AllocatorT>
	inline typename Array2D<T, AllocatorT>::size_type Array2D<T, AllocatorT>::count() const
	{
		return m_sizeX * m_sizeY;
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

	template <class T, class AllocatorT>
	inline T* Array2D<T, AllocatorT>::raw_data() const
	{
		return m_data;
	}

	template <class T, class AllocatorT>
	inline bool Array2D<T, AllocatorT>::is_valid_index(size_type x, size_type y) const
	{
		return x >= 0 && x < m_sizeX && y >= 0 && y < m_sizeY;
	}

	template <class T, class AllocatorT>
	inline bool Array2D<T, AllocatorT>::is_valid_index(size_type index) const
	{
		return index >= 0 && index < count();
	}

	template <class T, class AllocatorT>
	inline typename Array2D<T, AllocatorT>::size_type Array2D<T, AllocatorT>::to_index(size_type x, size_type y) const
	{
		FDK_ASSERT(is_valid_index(x, y));
		return y * size_x() + x;
	}

	template <class T, class AllocatorT>
	inline void Array2D<T, AllocatorT>::to_index(size_t index, size_type& x, size_type& y) const
	{
		FDK_ASSERT(is_valid_index(index));
		x = index % size_x();
		y = index / size_x();
	}
}

#endif
