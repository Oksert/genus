/**
 * author: Pavlov Anton 
 * created: 2006-11-15
 */


#pragma once

class RuEnStemmer : public StreamClass
{
protected:
	sb_stemmer *engStem;
	sb_stemmer *ruStem;
	StreamClass *input;
public:
    virtual std::string GetLexem() throw (StreamException);
    RuEnStemmer(StreamClass *in);
    ~RuEnStemmer();
};
