/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/10/28

*******************************************************************************/

#include "ParserStartup.h"
#include "HTMLParserIncludes.h"

using namespace HTMLParserLibrary;

UTFCaseChanger::UTFCaseChanger(unsigned int max_size)
{
    conversionArray = new unsigned int[max_size];
    size = max_size;

    for(unsigned int i = 0; i < size; i++)
    {
        conversionArray[i] = 0;
    }
}

UTFCaseChanger::UTFCaseChanger(unsigned int max_size, std::istream &input)
{
    conversionArray = new unsigned int[max_size];
    size = max_size;

    for(unsigned int i = 0; i < size; i++)
    {
        conversionArray[i] = 0;
    }
    ConversionLexer lexer(input);
    ConversionParser parser(lexer);
    parser.get_conversions(this);
}

UTFCaseChanger::UTFCaseChanger(unsigned int max_size, const char *_input)
{
    std::ifstream input(_input);
    conversionArray = new unsigned int[max_size];
    size = max_size;

    for(unsigned int i = 0; i < size; i++)
    {
        conversionArray[i] = 0;
    }
    ConversionLexer lexer(input);
    ConversionParser parser(lexer);
    parser.get_conversions(this);
}

UTFCaseChanger::~UTFCaseChanger()
{
    delete conversionArray;
}

unsigned int UTFCaseChanger::Convert(unsigned int val)
{
    if(val >= size)
        return val;
    if(conversionArray[val] == 0)
        return val;
    else
        return conversionArray[val];
}

void UTFCaseChanger::SetConversion(unsigned int from, unsigned int to)
{
    conversionArray[from] = to;
}
