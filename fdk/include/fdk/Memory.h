#ifndef __FDK_MEMORY_H_INCLUDE__
#define __FDK_MEMORY_H_INCLUDE__
#include <utility>

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
	inline void rawFillConstruct(T* p, const U& value)
	{
		new(p) T(value);
	}

	template <class ForwardIteratorT, class U>
	inline void rawFillConstructRange(ForwardIteratorT first, ForwardIteratorT last, const U& value)
	{
		ForwardIteratorT old = first;
		try
		{
			while (first != last)
			{
				rawFillConstruct(&*first, value);
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
	inline void rawFillConstructN(ForwardIteratorT first, SizeT n, const U& value)
	{
		ForwardIteratorT old = first;
		try
		{
			for (SizeT i = 0; i < n; ++i)
			{
				rawFillConstruct(&*first, value);
				++first;
			}
		}
		catch (...)
		{
			rawDestructRange(old, first);
			throw;
		}
	}
	
	template <class InputIteratorT, class ForwardIteratorT>
	inline void rawCopyConstructRange(InputIteratorT first, InputIteratorT last, ForwardIteratorT dest)
	{
		ForwardIteratorT old = dest;
		try
		{
			while (first != last)
			{
				rawFillConstruct(&*dest, *first);
				++first;
				++dest;
			}
		}
		catch (...)
		{
			rawDestructRange(old, dest);
			throw;
		}
	}

	template <class InputIteratorT, class SizeT, class ForwardIteratorT>
	inline void rawCopyConstructN(InputIteratorT first, SizeT n, ForwardIteratorT dest)
	{
		ForwardIteratorT old = dest;
		try
		{
			for (SizeT i = 0; i < n; ++i)
			{
				rawFillConstruct(&*dest, *first);
				++first;
				++dest;
			}
		}
		catch (...)
		{
			rawDestructRange(old, dest);
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
	inline void rawFillConstructByAllocator(T* p, const U& value, AllocatorT& allocator)
	{
		allocator.construct(p, value);
	}

	template <class ForwardIteratorT, class U, class AllocatorT>
	inline void rawFillConstructRangeByAllocator(ForwardIteratorT first, ForwardIteratorT last, const U& value, AllocatorT& allocator)
	{
		ForwardIteratorT old = first;
		try
		{
			while (first != last)
			{
				rawFillConstructByAllocator(&*first, value, allocator);
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
	inline void rawFillConstructNByAllocator(ForwardIteratorT first, SizeT n, const U& value, AllocatorT& allocator)
	{
		ForwardIteratorT old = first;
		try
		{
			for (SizeT i = 0; i < n; ++i)
			{
				rawFillConstructByAllocator(&*first, value, allocator);
				++first;
			}
		}
		catch (...)
		{
			rawDestructRangeByAllocator(old, first, allocator);
			throw;
		}
	}
	
	template <class InputIteratorT, class ForwardIteratorT, class AllocatorT>
	inline void rawCopyConstructRangeByAllocator(InputIteratorT first, InputIteratorT last, ForwardIteratorT dest, AllocatorT& allocator)
	{
		ForwardIteratorT old = dest;
		try
		{
			while (first != last)
			{
				rawFillConstructByAllocator(&*dest, *first, allocator);
				++first;
				++dest;
			}
		}
		catch (...)
		{
			rawDestructRangeByAllocator(old, dest, allocator);
			throw;
		}
	}

	template <class InputIteratorT, class SizeT, class ForwardIteratorT, class AllocatorT>
	inline void rawCopyConstructNByAllocator(InputIteratorT first, SizeT n, ForwardIteratorT dest, AllocatorT& allocator)
	{
		ForwardIteratorT old = dest;
		try
		{
			for (SizeT i = 0; i < n; ++i)
			{
				rawFillConstructByAllocator(&*dest, *first, allocator);
				++first;
				++dest;
			}
		}
		catch (...)
		{
			rawDestructRangeByAllocator(old, dest, allocator);
			throw;
		}
	}


	template <class SizeT, class AllocatorT>
	typename AllocatorT::pointer newDefaultByAllocator(SizeT n, AllocatorT& allocator)
	{
		typename AllocatorT::pointer ptr = allocator.allocate(n);
		if (!ptr)
		{
			throw std::bad_alloc();
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

	template <class SizeT, class U, class AllocatorT>
	typename AllocatorT::pointer newFillByAllocator(SizeT n, const U& value, AllocatorT& allocator)
	{
		typename AllocatorT::pointer ptr = allocator.allocate(n);
		if (!ptr)
		{
			throw std::bad_alloc();
		}
		try
		{
			rawFillConstructNByAllocator(ptr, n, value, allocator);
		}
		catch (...)
		{
			allocator.deallocate(ptr, n);
		}		
		return ptr;
	}

	template <class InputIteratorT, class AllocatorT>
	typename AllocatorT::pointer newCopyRangeByAllocator(InputIteratorT first, InputIteratorT last, AllocatorT& allocator)
	{
		typename AllocatorT::pointer ptr = allocator.allocate(n);
		if (!ptr)
		{
			throw std::bad_alloc();
		}
		try
		{
			rawCopyConstructRangeByAllocator(first, last, ptr, allocator);
		}
		catch (...)
		{
			allocator.deallocate(ptr, n);
		}		
		return ptr;
	}

	template <class InputIteratorT, class SizeT, class AllocatorT>
	typename AllocatorT::pointer newCopyNByAllocator(InputIteratorT first, SizeT n, AllocatorT& allocator)
	{
		typename AllocatorT::pointer ptr = allocator.allocate(n);
		if (!ptr)
		{
			throw std::bad_alloc();
		}
		try
		{
			rawCopyConstructNByAllocator(first, n, ptr, allocator);
		}
		catch (...)
		{
			allocator.deallocate(ptr, n);
		}		
		return ptr;
	}

	template <class ForwardIteratorT, class SizeT, class AllocatorT>
	void deleteNByAllocator(ForwardIteratorT first, SizeT n, AllocatorT& allocator)
	{
		rawDestructNByAllocator(first, n, allocator);
		allocator.deallocate(&*first, n);
	}

	template <class ForwardIteratorT, class AllocatorT>
	void deleteRangeByAllocator(ForwardIteratorT first, ForwardIteratorT last, AllocatorT& allocator)
	{
		rawDestructRangeByAllocator(first, last, allocator);
		allocator.deallocate(&*first, std::distance(first, last));
	}
}

#endif
