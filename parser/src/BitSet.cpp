/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/11/05

*******************************************************************************/

#include "ParserStartup.h"
#include "BitSet.h"

using namespace HTMLParserLibrary;

BitSet::BitSet(const unsigned long* bits_, unsigned int nlongs)
{
    size = nlongs;
    bitsArray = new unsigned int[size];
    for(unsigned int i = 0; i < size; i++)
    {
        bitsArray[i] = bits_[i];
    }
}

BitSet::~BitSet()
{
    delete bitsArray;
}

bool BitSet::member(unsigned int el) const
{
    unsigned int bit_index = el & 31;
    unsigned int number_index = el >> 5;
    if(number_index > size)
        return false;
    return bitsArray[number_index] & (1 << bit_index);
}

void BitSet::add(unsigned int el)
{
    // do nothing - not supported;
}

