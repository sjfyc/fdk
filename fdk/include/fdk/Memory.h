#ifndef __MEMORY_H
#define __MEMORY_H

namespace fdk
{
	template <class T>
	inline void rawDestruct(T* p)
	{
		p->~T();
	}

	template <class ForwardIteratorT>
	inline void rawDestructRange(ForwardIteratorT first, ForwardIteratorT last)
	{
		while (first != last)
		{
			rawDestruct(&*first);
			++first;
		}
	}

	template <class ForwardIteratorT, class SizeT>
	inline void rawDestructN(ForwardIteratorT first, SizeT n)
	{
		for (SizeT i = 0; i < n; ++i)
		{
			rawDestruct(&*first);
			++first;
		}
	}

	template <class T, class AllocatorT>
	inline void rawDestructByAllocator(T* p, AllocatorT& allocator)
	{
		allocator.destroy(p);
	}

	template <class ForwardIteratorT, class AllocatorT>
	inline void rawDestructRangeByAllocator(ForwardIteratorT first, ForwardIteratorT last, AllocatorT& allocator)
	{
		while (first != last)
		{
			rawDestructByAllocator(&*first, allocator);
			++first;
		}
	}

	template <class ForwardIteratorT, class SizeT, class AllocatorT>
	inline void rawDestructNByAllocator(ForwardIteratorT first, SizeT n, AllocatorT& allocator)
	{
		for (SizeT i = 0; i < n; ++i)
		{
			rawDestructByAllocator(&*first, allocator);
			++first;
		}
	}

	template <class T>
	inline void rawDefaultConstruct(T* p)
	{
		new(p) T();
	}	

	template <class ForwardIteratorT>
	inline void rawDefaultConstructRange(ForwardIteratorT first, ForwardIteratorT last)
	{
		ForwardIteratorT old = first;
		try
		{
			while (first != last)
			{
				rawDefaultConstruct(&*first);
				++first;
			}
		}
		catch (...)
		{
			rawDestructRange(old, first);
			throw;
		}
	}

	template <class ForwardIteratorT, class SizeT>
	inline void rawDefaultConstructN(ForwardIteratorT first, SizeT n)
	{
		ForwardIteratorT old = first;
		try
		{
			for (SizeT i = 0; i < n; ++i)
			{
				rawDefaultConstruct(&*first);
				++first;
			}
		}
		catch (...)
		{
			rawDestructRange(old, first);
			throw;
		}
	}

	template <class T, class U>
	inline void rawConstruct(T* p, const U& value)
	{
		new(p) T(value);
	}

	template <class ForwardIteratorT, class U>
	inline void rawConstructRange(ForwardIteratorT first, ForwardIteratorT last, const U& value)
	{
		ForwardIteratorT old = first;
		try
		{
			while (first != last)
			{
				rawConstruct(&*first, value);
				++first;
			}
		}
		catch (...)
		{
			rawDestructRange(old, first);
			throw;
		}
	}

	template <class ForwardIteratorT, class SizeT, class U>
	inline void rawConstructN(ForwardIteratorT first, SizeT n, const U& value)
	{
		ForwardIteratorT old = first;
		try
		{
			for (SizeT i = 0; i < n; ++i)
			{
				rawConstruct(&*first, value);
				++first;
			}
		}
		catch (...)
		{
			rawDestructRange(old, first);
			throw;
		}
	}

	template <class T, class AllocatorT>
	inline void rawDefaultConstructByAllocator(T* p, AllocatorT& allocator)
	{
		rawDefaultConstruct(p);
	}	

	template <class ForwardIteratorT, class AllocatorT>
	inline void rawDefaultConstructRangeByAllocator(ForwardIteratorT first, ForwardIteratorT last, AllocatorT& allocator)
	{
		ForwardIteratorT old = first;
		try
		{
			while (first != last)
			{
				rawDefaultConstructByAllocator(&*first, allocator);
				++first;
			}
		}
		catch (...)
		{
			rawDestructRangeByAllocator(old, first, allocator);
			throw;
		}
	}

	template <class ForwardIteratorT, class SizeT, class AllocatorT>
	inline void rawDefaultConstructNByAllocator(ForwardIteratorT first, SizeT n, AllocatorT& allocator)
	{
		ForwardIteratorT old = first;
		try
		{
			for (SizeT i = 0; i < n; ++i)
			{
				rawDefaultConstructByAllocator(&*first, allocator);
				++first;
			}
		}
		catch (...)
		{
			rawDestructRangeByAllocator(old, first, allocator);
			throw;
		}
	}

	template <class T, class U, class AllocatorT>
	inline void rawConstructByAllocator(T* p, const U& value, AllocatorT& allocator)
	{
		allocator.construct(p, value);
	}

	template <class ForwardIteratorT, class U, class AllocatorT>
	inline void rawConstructRangeByAllocator(ForwardIteratorT first, ForwardIteratorT last, const U& value, AllocatorT& allocator)
	{
		ForwardIteratorT old = first;
		try
		{
			while (first != last)
			{
				rawConstructByAllocator(&*first, value, allocator);
				++first;
			}
		}
		catch (...)
		{
			rawDestructRangeByAllocator(old, first, allocator);
			throw;
		}
	}

	template <class ForwardIteratorT, class SizeT, class U, class AllocatorT>
	inline void rawConstructNByAllocator(ForwardIteratorT first, SizeT n, const U& value, AllocatorT& allocator)
	{
		ForwardIteratorT old = first;
		try
		{
			for (SizeT i = 0; i < n; ++i)
			{
				rawConstructByAllocator(&*first, value, allocator);
				++first;
			}
		}
		catch (...)
		{
			rawDestructRangeByAllocator(old, first, allocator);
			throw;
		}
	}
	
	template <class SizeT, class AllocatorT>
	typename AllocatorT::pointer newDefaultByAllocator(SizeT n, AllocatorT& allocator)
	{
		typename AllocatorT::pointer ptr = allocator.allocate(n);
		if (!ptr)
		{
			throw std::bad_alloc()£»
		}
		try
		{
			rawDefaultConstructNByAllocator(ptr, n, allocator);
		}
		catch (...)
		{
			allocator.deallocate(ptr, n);
		}		
		return ptr;
	}
}

#endif