/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/11/05
    Description: 

*******************************************************************************/

#pragma once

namespace HTMLParserLibrary
{
    class BitSet
    {
    private:
        unsigned int *bitsArray;
        unsigned int size;
    protected:
    public:
        BitSet( const unsigned long* bits_, unsigned int nlongs);
        ~BitSet();
        void add( unsigned int el );
        bool member( unsigned int el ) const;
    };
}

