/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/11/26
    Description: 

*******************************************************************************/

#pragma once

class NumericFilter : public StreamClass
{
private:
	StreamClass *input;
public:
	std::string GetLexem() throw (StreamException)
	{
		while(true) {
			std::string ret_lex = input->GetLexem();
			if(!isdigit(ret_lex[0]))
				return ret_lex;
		}
	}
	NumericFilter(StreamClass *in)
	{
		input = in;
	}
};
