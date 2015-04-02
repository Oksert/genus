/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/12
    Description: 

*******************************************************************************/

/**
 * @file
 * 
 */

#pragma once

#include "Types.h"

namespace Similarity
{
	class Allocator
	{
	public:
		virtual void *alloc(size_t size) = 0;
		virtual void dealloc(void *ptr, size_t size) = 0;
		virtual ~Allocator();
	};

	template<class C> class AltAllocated
	{
	private:
		static Allocator *altAlloc;
		Allocator *actualAllocator;
	public:
		AltAllocated() {actualAllocator = altAlloc;};

		static void setAllocator(Allocator *a)
		{
			altAlloc = a;
		};

		void *operator new (size_t v)
		{
			if(altAlloc)
			{
				return altAlloc->alloc(v);
			}
			else
			{
				return ::operator new(v);
			}
		};
		void operator delete (void *ptr, size_t v)
		{
			if(((AltAllocated<C>*)ptr)->actualAllocator)
			{
				((AltAllocated<C>*)ptr)->actualAllocator->dealloc(ptr,v);
			}
			else
			{
				::operator delete(ptr);
			}
		};
	};
}
