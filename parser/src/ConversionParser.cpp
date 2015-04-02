/* $ANTLR 2.7.6 (2005-12-22): "conversion.g" -> "ConversionParser.cpp"$ */

#include "ParserStartup.h"
#include "HTMLParserIncludes.h"

ANTLR_BEGIN_NAMESPACE(HTMLParserLibrary)
ConversionParser::ConversionParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

ConversionParser::ConversionParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,1)
{
}

ConversionParser::ConversionParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

ConversionParser::ConversionParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,1)
{
}

ConversionParser::ConversionParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,1)
{
}

void ConversionParser::get_conversions(
	UTFCaseChanger *chng
) {
	
	changer = chng;
	
	
	try {      // for error handling
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == NUMBER)) {
				conversion();
			}
			else {
				goto _loop3;
			}
			
		}
		_loop3:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void ConversionParser::conversion() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  v_from = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v_to = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	unsigned int from;
	unsigned int to;
	bool add = false;
	
	
	try {      // for error handling
		v_from = LT(1);
		match(NUMBER);
		{
		switch ( LA(1)) {
		case C_TYPE:
		{
			match(C_TYPE);
			add = true;
			break;
		}
		case S_TYPE:
		{
			match(S_TYPE);
			add = true;
			break;
		}
		case T_TYPE:
		{
			match(T_TYPE);
			break;
		}
		case F_TYPE:
		{
			match(F_TYPE);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		v_to = LT(1);
		match(NUMBER);
		
		if(add)
		{
		from = toHexInt(v_from->getText());
		to = toHexInt(v_to->getText());
		changer->SetConversion(from,to);
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void ConversionParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* ConversionParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"NUMBER",
	"C_TYPE",
	"S_TYPE",
	"T_TYPE",
	"F_TYPE",
	"WS",
	"COMMENT",
	"HEX_DIGIT",
	0
};

const unsigned long ConversionParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet ConversionParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long ConversionParser::_tokenSet_1_data_[] = { 18UL, 0UL, 0UL, 0UL };
// EOF NUMBER 
const ANTLR_USE_NAMESPACE(antlr)BitSet ConversionParser::_tokenSet_1(_tokenSet_1_data_,4);


ANTLR_END_NAMESPACE
