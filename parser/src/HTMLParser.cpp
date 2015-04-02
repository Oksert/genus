/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/11/01

*******************************************************************************/

#include "ParserStartup.h"
#include "HTMLParserIncludes.h"

using namespace HTMLParserLibrary;

HTMLParser::HTMLParser(UTFCaseChanger &_changer)
:changer(_changer)
{
	errors = 0;
    lexer = NULL;
    convBuffer = NULL;
}

void HTMLParser::SetStream(ByteStream *str)
{
    byteStream = str;

    if(lexer != NULL) delete lexer;
    if(convBuffer != NULL) delete convBuffer;
    convBuffer = new UnicodeCharBufferConv(*byteStream,changer);
    lexer = new HTMLLexer(*convBuffer);
    lexer->done = false;
}

std::string HTMLParser::GetLexem() throw (StreamException)
{
    if(lexer == NULL)
    {
        // wrong call
        throw new std::exception;
    }
    std::string ret = load();
    return ret;
}

std::string HTMLParser::load()
{
	//struct timespec startTime1; 
	//clock_gettime(CLOCK_REALTIME, &startTime1);

	antlr::RefToken t;
	bool success = false;
	while(!success)
	{
		try
		{
			t = lexer->nextToken();
			// if no errors occured - break the loop.
			success = true;
		}
		catch(...)
		{
			
			if(lexer->done) break;
			// discard bad segment
			lexer->consume();
			errors++;
			// do nothing - continue
		}
	}
	
	if(!success)
		throw EndOfStreamException();
	
    if(t->getType() != HTMLLexer::WORD)
    {
    	if(t->getType() == HTMLLexer::A_TAG)
    	{
    		throw AException(t->getText());
    	}
    	else if(t->getType() == HTMLLexer::BASE_TAG)
    	{
    		throw BASEException(t->getText());
    	}
    	else if(t->getType() == HTMLLexer::IMG_TAG)
    	{
    		throw IMGException(t->getText());
    	}
    	else if(t->getType() == HTMLLexer::PUNCTUATION_MARK)
    	{
    		throw PunctException(t->getText());
    	}
		else if(t->getType() == HTMLLexer::END_OF_CHUNK)
    	{
    		throw EndOfChunkException();
    	}
    	else
    	{
    		// forbidden symbol
    		throw EndOfStreamException();
    	}
    }

    //struct timespec endTime1; 
	//clock_gettime(CLOCK_REALTIME, &endTime1);
	//std::cout<<"ParsingLoadTime=" << endTime1.tv_sec-startTime1.tv_sec<<" "<<endTime1.tv_nsec-startTime1.tv_nsec<<std::endl;
	
	return t->getText();
}

HTMLParser::~HTMLParser()
{
    if(lexer != NULL) delete lexer;
    if(convBuffer != NULL) delete convBuffer;
}

size_t HTMLParser::GetNumberOfErrors()
{
	return errors;
}
