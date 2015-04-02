#ifndef INC_HTMLLexer_hpp_
#define INC_HTMLLexer_hpp_

// Include correct superclass header with a header statement for example:
// header "post_include_hpp" {
// #include "UnicodeCharScanner.hpp"
// }
// Or....
// header {
// #include "UnicodeCharScanner.hpp"
// }

ANTLR_BEGIN_NAMESPACE(HTMLParserLibrary)

class CUSTOM_API HTMLLexer : public UnicodeCharScanner, public HTMLLexerTokenTypes
{

protected:
    int in_pre_section;
public:
	bool done;

	HTMLLexer( ConvertedStream& in, UTFCaseChanger &changer )
	: UnicodeCharScanner(new UnicodeCharBufferConv(in,changer),true)
	{
	    in_pre_section = 0;
		initLiterals();
	}
	HTMLLexer( UnicodeCharBufferConv& ib )
	: UnicodeCharScanner(ib,true)
	{
	    in_pre_section = 0;
		initLiterals();
	}

	void uponEOF()
	{
		done = true;
	}
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return true;
	}
public:
#if 0
// constructor creation turned of with 'noConstructor' option
	HTMLLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	HTMLLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	HTMLLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
// constructor creation turned of with 'noConstructor' option
#endif
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	protected: void mTAG_ID(bool _createToken);
	protected: void mID_LETTER(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	protected: void mATTR_ID(bool _createToken);
	public: void mEND_OF_CHUNK(bool _createToken);
	public: void mWS(bool _createToken);
	public: void mWORD(bool _createToken);
	public: void mESC(bool _createToken);
	public: void mPUNCTUATION_MARK(bool _createToken);
	public: void mFILTER_SYMBOL(bool _createToken);
	protected: void mHREF_NAME(bool _createToken);
	protected: void mSRC_NAME(bool _createToken);
	protected: void mBASE_ATTR_LIST(bool _createToken);
	protected: void mSTRING(bool _createToken);
	protected: void mSKIP_ATTR_LIST(bool _createToken);
	protected: void mSTYLE_NAME(bool _createToken);
	protected: void mBASE_NAME(bool _createToken);
	protected: void mSCRIPT_NAME(bool _createToken);
	protected: void mIMG_NAME(bool _createToken);
	protected: void mA_NAME(bool _createToken);
	protected: void mPRE_NAME(bool _createToken);
	protected: void mDOCTYPE_NAME(bool _createToken);
	public: void mTAG(bool _createToken);
	protected: void mDOCTYPE_CONTENT(bool _createToken);
	protected: void mCOMMENT_CONTENT(bool _createToken);
	protected: void mSTYLE_CONTENT(bool _createToken);
	protected: void mSCRIPT_CONTENT(bool _createToken);
	protected: void mIMG_ATTR_LIST(bool _createToken);
	protected: void mA_ATTR_LIST(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const BitSet _tokenSet_4;
	static const unsigned long _tokenSet_5_data_[];
	static const BitSet _tokenSet_5;
	static const unsigned long _tokenSet_6_data_[];
	static const BitSet _tokenSet_6;
	static const unsigned long _tokenSet_7_data_[];
	static const BitSet _tokenSet_7;
	static const unsigned long _tokenSet_8_data_[];
	static const BitSet _tokenSet_8;
	static const unsigned long _tokenSet_9_data_[];
	static const BitSet _tokenSet_9;
	static const unsigned long _tokenSet_10_data_[];
	static const BitSet _tokenSet_10;
	static const unsigned long _tokenSet_11_data_[];
	static const BitSet _tokenSet_11;
	static const unsigned long _tokenSet_12_data_[];
	static const BitSet _tokenSet_12;
	static const unsigned long _tokenSet_13_data_[];
	static const BitSet _tokenSet_13;
	static const unsigned long _tokenSet_14_data_[];
	static const BitSet _tokenSet_14;
	static const unsigned long _tokenSet_15_data_[];
	static const BitSet _tokenSet_15;
	static const unsigned long _tokenSet_16_data_[];
	static const BitSet _tokenSet_16;
	static const unsigned long _tokenSet_17_data_[];
	static const BitSet _tokenSet_17;
};

ANTLR_END_NAMESPACE
#endif /*INC_HTMLLexer_hpp_*/
