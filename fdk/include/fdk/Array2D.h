#ifndef __FDK_ARRAY2D_H_INCLUDE__
#define __FDK_ARRAY2D_H_INCLUDE__
#include "Base.h"
#include <stdexcept>

namespace fdk
{
	template <class T>
	class Array2D
	{
	public:
		typedef size_t size_type;
		typedef T value_type;
		Array2D(size_type sizeX, size_type sizeY);
		Array2D(size_type sizeX, size_type sizeY, const T& value);
		Array2D(const Array2D& other);
		~Array2D();		
		T& operator()(size_type x, size_type y);
		const T& operator()(size_type x, size_type y) const;
		size_type sizeX() const;
		size_type sizeY() const;
	private:
		size_type m_sizeX;
		size_type m_sizeY;
		T* m_data;
	};

	template <class T>
	Array2D<T>::Array2D(size_type sizeX, size_type sizeY)
		: m_sizeX(sizeX)
		, m_sizeY(sizeY)
		, m_data((T*)malloc(sizeof(T)*sizeX*sizeY))
	{
		if (!m_data)
		{
			throw std::runtime_error("Array2D(sizeX, sizeY) out of memory");
		}
		FDK_ASSERT(m_sizeY > 0 && m_sizeY > 0);
		for (size_type i = 0; i < m_sizeX*m_sizeY; ++i)
		{	
			rawConstruct(&m_data[i]);
		}
	}

	template <class T>
	Array2D<T>::Array2D(size_type sizeX, size_type sizeY, const T& value)
		: m_sizeX(sizeX)
		, m_sizeY(sizeY)
		, m_data((T*)malloc(sizeof(T)*sizeX*sizeY))
	{
		if (!m_data)
		{
			throw std::runtime_error("Array2D(sizeX, sizeY, value)  out of memory");
		}
		FDK_ASSERT(m_sizeY > 0 && m_sizeY > 0);
		for (size_type i = 0; i < m_sizeX*m_sizeY; ++i)
		{
			rawConstruct(&m_data[i], value);
		}
	}

	template <class T>
	Array2D<T>::Array2D(const Array2D& other)
		: m_sizeX(other.m_sizeX)
		, m_sizeY(other.m_sizeY)
		, m_data((T*)malloc(sizeof(T)*other.m_sizeX*other.m_sizeY))
	{
		if (!m_data)
		{
			throw std::runtime_error("Array2D copy constructor out of memory");
		}
		for (size_type i = 0; i < m_sizeX*m_sizeY; ++i)
		{
			rawConstruct(&m_data[i], other.m_data[i]);
		}		
	}

	template <class T>
	Array2D<T>::~Array2D()
	{	
		rawDestroy(m_data, m_data+m_sizeX*m_sizeY);
		free(m_data);
	}
	
	template <class T>
	inline T& Array2D<T>::operator()(size_type x, size_type y)
	{
		return const_cast<T&>(
			static_cast<const Array2D<T>&>(*this)(x, y)
			);
	}

	template <class T>
	inline const T& Array2D<T>::operator()(size_type x, size_type y) const
	{
		FDK_ASSERT(x >= 0 && x < m_sizeX);
		FDK_ASSERT(y >= 0 && y < m_sizeY);
		return m_data[y*m_sizeX+x];
	}

	template <class T>
	inline typename Array2D<T>::size_type Array2D<T>::sizeX() const
	{
		return m_sizeX;
	}

	template <class T>
	inline typename Array2D<T>::size_type Array2D<T>::sizeY() const
	{
		return m_sizeY;
	}
}

#endif
