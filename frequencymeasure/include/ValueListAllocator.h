/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2007/03/11
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
    /**
     * The class, that provides special allocation strategy for
     * Similarity::SparseDocumentOld::ValueList structure.
     *
     * @note Use fast allocation method for reading FrequencyMeasure
     * from a binary file.
     *
     * @note Use common allocation method when reading data from preloaded
     * set.
     *
     * @note Use only one allocation method throughout the lifetime of
     * FrequencyMeasure class!
     */
    class ValueListAllocator
    {
    private:
    protected:
        static ValueListAllocator *singleton;
        static size_t elementsNumber;
        static size_t structSize;

        ValueListAllocator();
        ~ValueListAllocator();

        size_t newCounter;
        size_t deleteCounter;
        struct Block 
        {
            size_t freeIndex;
            char *memory;
            Block *next;
            Block(size_t size = 0);
            ~Block();
        };

        Block blockList;
        Block *lastPtr;

        static ValueListAllocator *getSingleton();
    public:
        /**
         * Allocates a chunk of memory.
         */
        static void *alloc(size_t);
        /**
         * Dealocates the previously allocated chunk.
         */
        static void dealloc(void *ptr, size_t);
        /**
         * Dealocates all allocated chunks.
         */
        static void destroyAllInstances();

        /**
         * Sets fast allocation method. (useful for reading frequency
         * measure from file).
         */
        static void setFastAllocationMethod();
        /**
         * Sets common allocation method.
         */
        static void setCommonAllocationMethod();
    };
}

