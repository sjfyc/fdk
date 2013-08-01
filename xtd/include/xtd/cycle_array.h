#ifndef __XTD_CYCLE_ARRAY_H_INCLUDE__
#define __XTD_CYCLE_ARRAY_H_INCLUDE__
#include "_base.h"

namespace xtd
{
	template <class T> 
	class cycle_array
	{
	public:
		typedef size_t size_type;
		typedef T value_type;
		cycle_array(size_type maxSize);
		~cycle_array();
		T& operator[](size_type index);
		const T& operator[](size_type index) const;		
		void clear();
		void push_front(const T& value);
		void push_back(const T& value);
		void pop_front();
		void pop_back();
		// @require: maxSize >= size()
		void adjust_max_size(size_type maxSize);
		size_type max_size() const;
		size_type size() const;
		bool empty() const;
	private:
		T* m_array;		
		size_type m_maxSize;
		size_type m_size;
		size_type m_frontIndex;
	};

	template <class T>
	cycle_array<T>::cycle_array(size_type maxSize)
		: m_array(new T[maxSize])
		, m_maxSize(maxSize)
		, m_size(0)
		, m_frontIndex(0)
	{
		XTD_ASSERT(maxSize > 0);
	}

	template <class T>
	cycle_array<T>::~cycle_array()
	{
		delete[] m_array;
	}
	
	template <class T>
	inline T& cycle_array<T>::operator[](size_type index)
	{
		XTD_ASSERT(index < m_size);
		return m_array[ (m_frontIndex+index) % m_maxSize ];
	}
	
	template <class T>
	inline const T& cycle_array<T>::operator[](size_type index) const
	{
		XTD_ASSERT(index < m_size);
		return m_array[ (m_frontIndex+index) % m_maxSize ];
	}
	
	template <class T>
	void cycle_array<T>::clear()
	{
		m_size = 0;
		m_frontIndex = 0;
	}

	template <class T>
	void cycle_array<T>::push_front(const T& value)
	{
		m_frontIndex = (m_frontIndex+m_maxSize-1) % m_maxSize;
		m_array[m_frontIndex] = value;
		if(m_size < m_maxSize)
		{
			++m_size;
		}
	}

	template <class T>
	void cycle_array<T>::push_back(const T& value)
	{
		m_array[ (m_frontIndex+m_size) % m_maxSize ] = value;
		if(m_size < m_maxSize)
		{
			++m_size;
		}
		else
		{
			m_frontIndex = (m_frontIndex+1) % m_maxSize;
		}
	}

	template <class T>
	void cycle_array<T>::pop_front()
	{
		XTD_ASSERT(!empty());
		m_frontIndex = (m_frontIndex+1) % m_maxSize;
		--m_size;
	}

	template <class T>
	void cycle_array<T>::pop_back()
	{
		XTD_ASSERT(!empty());
		--m_size;
	}

	template <class T>
	void cycle_array<T>::adjust_max_size(size_type maxSize)
	{
		if (maxSize == m_maxSize)
		{
			return;
		}
		XTD_ASSERT(size() <= maxSize);
		T* newArray = new T[maxSize];
		const size_type frontCount = m_maxSize - m_frontIndex;
		const size_type frontCopyCount = (frontCount > m_size) ? m_size : frontCount;
		size_type copiedCount = 0;
		size_type copyIndex;
		for (copyIndex = m_frontIndex; copiedCount < frontCopyCount; ++copiedCount, ++copyIndex)
		{
			newArray[copiedCount] = m_array[copyIndex];
		}
		for (copyIndex = 0; copiedCount < m_size; ++copiedCount, ++copyIndex)
		{
			newArray[copiedCount] = m_array[copyIndex];
		}
		delete[] m_array;
		m_array = newArray;
		m_maxSize = maxSize;
		m_frontIndex = 0;
	}

	template <class T>
	inline typename cycle_array<T>::size_type cycle_array<T>::max_size() const
	{
		return m_maxSize;
	}

	template <class T>
	inline typename cycle_array<T>::size_type cycle_array<T>::size() const
	{
		return m_size;
	}

	template <class T>
	inline bool cycle_array<T>::empty() const
	{
		return m_size == 0;
	}
}

#endif
