/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2007/03/11

*******************************************************************************/

#include "ValueListAllocator.h"
#include "FrequencyMeasure.h"

size_t Similarity::ValueListAllocator::structSize = sizeof(Similarity::SparseDocumentOld::ValueList);
size_t Similarity::ValueListAllocator::elementsNumber = 4096;
Similarity::ValueListAllocator *Similarity::ValueListAllocator::singleton = NULL;

using namespace Similarity;

ValueListAllocator::ValueListAllocator()
{
    newCounter = 0;
    deleteCounter = 0;
    lastPtr = &blockList;
}

ValueListAllocator::~ValueListAllocator()
{
    while(blockList.next != NULL)
    {
        Block *tmp = blockList.next;
        blockList.next = blockList.next->next;
        delete tmp;
    }
}
ValueListAllocator::Block::Block(size_t size)
{
    if(size)
    {
        memory = new char[size];
        memset(memory,0xff,size);
    }
    else
        memory = NULL;
    next = NULL;
    freeIndex = 0;
}

ValueListAllocator::Block::~Block()
{
    if(memory)
        delete memory;
}

ValueListAllocator *ValueListAllocator::getSingleton()
{
    if(singleton != NULL) return singleton;
    singleton = new ValueListAllocator;
    return singleton;
}

void *ValueListAllocator::alloc(size_t)
{
    ValueListAllocator *ptr = getSingleton();
    if(ptr->lastPtr->next == NULL)
    {
        ptr->lastPtr->next = new Block(elementsNumber * structSize);
    }

    void *ret = ((char*)ptr->lastPtr->next->memory + ptr->lastPtr->next->freeIndex * structSize);
    ptr->lastPtr->next->freeIndex++;

    if(ptr->lastPtr->next->freeIndex >= elementsNumber)
    {
        ptr->lastPtr = ptr->lastPtr->next;
    }

    ptr->newCounter++;

    return ret;
}

void ValueListAllocator::dealloc(void *ptr, size_t)
{
    ValueListAllocator *al = getSingleton();
    al->deleteCounter++;
    // do nothing for now
}

void ValueListAllocator::destroyAllInstances()
{
    delete singleton;
    singleton = NULL;
}

void ValueListAllocator::setFastAllocationMethod()
{
    Similarity::SparseDocumentOld::ValueList::operator_new = Similarity::ValueListAllocator::alloc;
    Similarity::SparseDocumentOld::ValueList::operator_delete = Similarity::ValueListAllocator::dealloc;
}

void ValueListAllocator::setCommonAllocationMethod()
{
    Similarity::SparseDocumentOld::ValueList::operator_new = Similarity::SparseDocumentOld::ValueList::__operator_new;
    Similarity::SparseDocumentOld::ValueList::operator_delete = Similarity::SparseDocumentOld::ValueList::__operator_delete;
}
