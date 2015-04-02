/*******************************************************************************

Author:      Pavlov Anton
Created:     2006/11/01
Description: 

*******************************************************************************/

#pragma once

namespace HTMLParserLibrary
{
    class HTMLParser : public StreamClass
    {
    private:
        UTFCaseChanger &changer;
        ByteStream *byteStream;
        UnicodeCharBufferConv *convBuffer;
        HTMLLexer *lexer;

		std::string load();
        size_t errors;
    protected:
    public:
        virtual std::string GetLexem() throw (StreamException);
        void SetStream(ByteStream *str);
        HTMLParser(UTFCaseChanger &_changer);
        virtual ~HTMLParser();
        
        size_t GetNumberOfErrors();
    };
}
