/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/11/02
    Description: 

*******************************************************************************/

#pragma once

class StopWordsFilter : public StreamClass
{
private:
	StreamClass *input;
	DictionaryDB::DictionaryInterface &stop;
public:
	std::string GetLexem() throw (StreamException)
	{
		while(true)
		{
			std::string lex = input->GetLexem();
			if(!stop.Exists(lex))
			{
				return lex;
			}
		}
	}
	StopWordsFilter(StreamClass *in, DictionaryDB::DictionaryInterface &dict)
	:stop(dict)
	{
		input = in;
	}
};
