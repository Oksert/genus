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
#include "AlternativeAllocator.h"

namespace Similarity
{
	template<class C, int blockSize = 1024> class BlockAllocator : public Allocator
	{
	private:

		size_t newCounter;
		size_t deleteCounter;
		struct Block 
		{
			size_t freeIndex;
			size_t size;
			char *memory;
			Block *next;
			inline void *getNextBatch(size_t amount)
			{
				void *res = memory + freeIndex;
				freeIndex += amount;
				return res;
			}
			inline bool isEmpty()
			{
				return freeIndex >= size;
			}
			Block(size_t sz = 0)
			{
				size = sz;
				memory = new char[size];
				freeIndex = 0;
				next = NULL;
			};
			~Block()
			{
				delete memory;
			};
		};

		Block blockList;
		Block *lastPtr;

		BlockAllocator()
		{
			init();
		};

		Block *createNewBlock()
		{
			return new Block(blockSize * sizeof(C));
		}

		void init()
		{
			newCounter = 0;
			deleteCounter = 0;
			blockList.next = createNewBlock();
			lastPtr = blockList.next;
		}

		void clean()
		{
			Block *ptr = blockList.next;
			while(ptr != NULL)
			{
				Block *tmp = ptr->next;
				delete ptr;
				ptr = tmp;
			}
		}

		virtual ~BlockAllocator()
		{
			clean();
		};

		static BlockAllocator<C,blockSize> *singleton;
	public:
		static BlockAllocator<C,blockSize> *getSingleton()
		{
			if(!singleton)
			{
				singleton = new BlockAllocator<C,blockSize>();
			}
			return singleton;
		};
		virtual void *alloc(size_t size)
		{
			newCounter++;
			if(lastPtr->isEmpty())
			{
				lastPtr->next = createNewBlock();
				lastPtr = lastPtr->next;
			}
			return lastPtr->getNextBatch(size);
		};
		virtual void dealloc(void *ptr, size_t size)
		{
			deleteCounter++;
			if(getLiveInstancesCount() == 0)
			{
				freeMemory();
			}
		};

		static void freeMemory()
		{
			singleton->clean();
			singleton->init();
		};

		static size_t getLiveInstancesCount()
		{
			BlockAllocator<C,blockSize> *p = getSingleton();
			return p->newCounter - p->deleteCounter;
		};
	};
	template<class C, int blockSize> BlockAllocator<C,blockSize> *BlockAllocator<C,blockSize>::singleton = NULL;
}
