#include "ParserStartup.h"
#include <libstemmer.h>
#include "StreamClass.h"
#include "Stemmer.h"

std::string RuEnStemmer::GetLexem() throw (StreamException)
{
	std::string tmp = input->GetLexem();
	std::string buffer = "";
	if(int(tmp[0]) >= 0)	// invoking English stemmer
	{
		const char *ptr = (const char*)sb_stemmer_stem(engStem,(const sb_symbol *)(tmp.c_str()),tmp.length());
		int length = sb_stemmer_length(engStem);
		
		buffer.clear();
		for(int i = 0; i < length; i++)
		{
			buffer.push_back(ptr[i]);
		}
	}
	else					// invoking Russian stemmer
	{
		const char *ptr = (const char*)sb_stemmer_stem(ruStem,(const sb_symbol *)(tmp.c_str()),tmp.length());
		int length = sb_stemmer_length(ruStem);
		
		buffer.clear();
		for(int i = 0; i < length; i++)
		{
			buffer.push_back(ptr[i]);
		}
	}
	return buffer;
}


RuEnStemmer::RuEnStemmer(StreamClass *inp)
{
	input = inp;
	engStem = sb_stemmer_new("eng","UTF_8");
	ruStem = sb_stemmer_new("ru","UTF_8");
}

RuEnStemmer::~RuEnStemmer()
{
	sb_stemmer_delete(engStem);
	sb_stemmer_delete(ruStem);
}
