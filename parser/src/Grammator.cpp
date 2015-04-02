
#include "ParserStartup.h"
#include "StreamClass.h"
#include "Grammator.h"

#define ONE_BYTE  (0)
#define TWO_BYTES  (-64)
#define THREE_BYTES  (-32)
#define FOUR_BYTES  (-16)

using namespace std;


Grammator::Grammator(int n)
{
	dimension = n;
}

Grammator::~Grammator()
{}

void Grammator::SetStream(StreamClass* stream_obj)
{
	stream = stream_obj;
}

std::string Grammator::GetLexem() throw (StreamException)
{
	if(cached_gramms.size() == 0)
	{
		std::string newWord = stream->GetLexem();
		cache(newWord);
	}
	std::string ret = *(cached_gramms.begin());
	cached_gramms.pop_front();
	return ret;
}

void Grammator::cache(std::string word)
{
	std::vector<std::string> symbols;
	int bytes_read = 1;
	for(int i = 0; i < word.size(); i += bytes_read)
	{
		std::string newS = "";
		if(i > 0)
		{
			newS.push_back(word[i]);
			symbols.push_back(newS);
			bytes_read = 1;
		}
		else if (i > FOUR_BYTES)
		{
			newS.push_back(word[i]);
			newS.push_back(word[i+1]);
			newS.push_back(word[i+2]);
			newS.push_back(word[i+3]);
			symbols.push_back(newS);
			bytes_read = 4;
		}
		else if (i > THREE_BYTES)
		{
			newS.push_back(word[i]);
			newS.push_back(word[i+1]);
			newS.push_back(word[i+2]);
			symbols.push_back(newS);
			bytes_read = 3;
		}
		else if (i > TWO_BYTES)
		{
			newS.push_back(word[i]);
			newS.push_back(word[i+1]);
			symbols.push_back(newS);
			bytes_read = 2;
		}
	}
	
	if(symbols.size() <= dimension)
	{
		cached_gramms.push_back(word);
		return;
	}
	
	for(int i = 0; i < symbols.size() - dimension; i++)
	{
		std::string constrS = "";
		for(int l = 0; l < dimension; l++)
		{
			constrS += symbols[i+l];
		}
		cached_gramms.push_back(constrS);
	}
}
