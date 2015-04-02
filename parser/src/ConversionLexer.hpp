
#pragma once

ANTLR_BEGIN_NAMESPACE(HTMLParserLibrary)

class CUSTOM_API ConversionLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public ConversionParserTokenTypes
{
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return true;
	}
public:
	ConversionLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	ConversionLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	ConversionLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	public: void mWS(bool _createToken);
	public: void mCOMMENT(bool _createToken);
	public: void mNUMBER(bool _createToken);
	protected: void mHEX_DIGIT(bool _createToken);
	public: void mC_TYPE(bool _createToken);
	public: void mS_TYPE(bool _createToken);
	public: void mT_TYPE(bool _createToken);
	public: void mF_TYPE(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
};

ANTLR_END_NAMESPACE
