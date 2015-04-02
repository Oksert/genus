#pragma once

#include <cstdlib>

ANTLR_BEGIN_NAMESPACE(HTMLParserLibrary)

class CUSTOM_API ConversionParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public ConversionParserTokenTypes
{

UTFCaseChanger *changer;
unsigned int toHexInt(std::string str)
{
    return (unsigned int)strtol(str.c_str(),NULL,16);
}
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	ConversionParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	ConversionParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	ConversionParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	ConversionParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	ConversionParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return ConversionParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return ConversionParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return ConversionParser::tokenNames;
	}
	public: void get_conversions(
		UTFCaseChanger *chng
	);
	public: void conversion();
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 12;
#else
	enum {
		NUM_TOKENS = 12
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
};

ANTLR_END_NAMESPACE
