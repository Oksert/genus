/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/10/28
    Description: 

*******************************************************************************/

#pragma once

namespace HTMLParserLibrary
{
    class UTFCaseChanger
    {
    private:
        unsigned int size;
        unsigned int *conversionArray;
    protected:
    public:
        unsigned int Convert(unsigned int val);
        void SetConversion(unsigned int from, unsigned int to);
        UTFCaseChanger(unsigned int max_size, std::istream &input);
        UTFCaseChanger(unsigned int max_size, const char *input);
        UTFCaseChanger(unsigned int max_size);
        ~UTFCaseChanger();
    };
}

