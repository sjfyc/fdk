#ifndef __FDK_ITERATOR_H_INCLUDE__
#define __FDK_ITERATOR_H_INCLUDE__
#include "Base.h"

namespace fdk
{
	namespace _iterator
	{
		template <class T> 
		struct GetValueTypeOfPointer;

		template <class T>
		struct GetValueTypeOfPointer<T*>
		{
			typedef T ValueType;
		};

		template <class T> 
		struct GetConstValueType
		{
			typedef const T ConstValueType;
		};

		template <class T> 
		struct GetConstValueType<const T>
		{
			typedef const T ConstValueType;
		};

		template <class PointerIteratorT>
		struct GetOrignValueTypeOfPointerIterator
		{
		private:
			typedef typename std::iterator_traits<PointerIteratorT>::value_type PointerType;
		public:
			typedef typename GetValueTypeOfPointer<PointerType>::ValueType ValueType;
		};
	}

	// 把指向指针的迭代器转换为指向值的迭代器
	template <class IteratorT>
	class PointerIteratorToConstValueIterator
		: public std::iterator<
		std::forward_iterator_tag, 
		typename _iterator::GetConstValueType<
		typename _iterator::GetOrignValueTypeOfPointerIterator<IteratorT>::ValueType
		>::ConstValueType
		>
	{
		typedef PointerIteratorToConstValueIterator _Self;
	public:
		PointerIteratorToConstValueIterator();
		PointerIteratorToConstValueIterator(IteratorT it);
		reference operator*() const;
		pointer operator->() const;
		_Self& operator++();
		_Self operator++(int);
		bool operator==(const _Self& other) const;
		bool operator!=(const _Self& other) const;
	private:
		IteratorT m_it;
	};

	template <class IteratorT>
	class PointerIteratorToValueIterator
		: public PointerIteratorToConstValueIterator<IteratorT>
	{
		typedef PointerIteratorToValueIterator<IteratorT> _Self;
		typedef PointerIteratorToConstValueIterator<IteratorT> _Base;
	public:
		typedef typename _iterator::GetOrignValueTypeOfPointerIterator<IteratorT>::ValueType value_type;
		typedef value_type& reference;
		typedef value_type* pointer;
		PointerIteratorToValueIterator();
		PointerIteratorToValueIterator(IteratorT it);
		reference operator*() const;
		pointer operator->() const;
		_Self& operator++();
		_Self operator++(int);
	};

	template <class IteratorT>
	inline PointerIteratorToConstValueIterator<IteratorT>::PointerIteratorToConstValueIterator() 
		: m_it()
	{
	}

	template <class IteratorT>
	inline PointerIteratorToConstValueIterator<IteratorT>::PointerIteratorToConstValueIterator(IteratorT it)
		: m_it(it)
	{
	}

	template <class IteratorT>
	inline typename PointerIteratorToConstValueIterator<IteratorT>::reference PointerIteratorToConstValueIterator<IteratorT>::operator*() const
	{
		 return **m_it; 
	}

	template <class IteratorT>
	inline typename PointerIteratorToConstValueIterator<IteratorT>::pointer PointerIteratorToConstValueIterator<IteratorT>::operator->() const
	{
		return &**this;
	}

	template <class IteratorT>
	inline PointerIteratorToConstValueIterator<IteratorT>& PointerIteratorToConstValueIterator<IteratorT>::operator++()
	{
		++m_it; 
		return *this;
	}

	template <class IteratorT>
	inline PointerIteratorToConstValueIterator<IteratorT> PointerIteratorToConstValueIterator<IteratorT>::operator++(int)
	{
		_Self temp = *this; 
		++*this; 
		return temp;
	}

	template <class IteratorT>
	inline bool PointerIteratorToConstValueIterator<IteratorT>::operator==(const _Self& other) const
	{
		return m_it == other.m_it;
	}

	template <class IteratorT>
	inline bool PointerIteratorToConstValueIterator<IteratorT>::operator!=(const _Self& other) const
	{
		return m_it != other.m_it;
	}

	template <class IteratorT>
	inline PointerIteratorToValueIterator<IteratorT>::PointerIteratorToValueIterator()
		: _Base()
	{
	}

	template <class IteratorT>
	inline PointerIteratorToValueIterator<IteratorT>::PointerIteratorToValueIterator(IteratorT it)
		: _Base(it)
	{
	}
	
	template <class IteratorT>
	inline typename PointerIteratorToValueIterator<IteratorT>::reference PointerIteratorToValueIterator<IteratorT>::operator*() const
	{
		return (reference)*(_Base&)*this;
	}

	template <class IteratorT>
	inline typename PointerIteratorToValueIterator<IteratorT>::pointer PointerIteratorToValueIterator<IteratorT>::operator->() const
	{
		return &**this;
	}

	template <class IteratorT>
	inline PointerIteratorToValueIterator<IteratorT>& PointerIteratorToValueIterator<IteratorT>::operator++()
	{
		++(_Base&)*this; 
		return *this;
	}

	template <class IteratorT>
	inline PointerIteratorToValueIterator<IteratorT> PointerIteratorToValueIterator<IteratorT>::operator++(int)
	{
		_Self temp = *this; 
		++*this; 
		return temp;
	}
}

#endif