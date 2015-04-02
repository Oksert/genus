#ifndef INC_HTMLLexerTokenTypes_hpp_
#define INC_HTMLLexerTokenTypes_hpp_

ANTLR_BEGIN_NAMESPACE(HTMLParserLibrary)
/* $ANTLR 2.7.6 (2005-12-22): "unicode_html.g" -> "HTMLLexerTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API HTMLLexerTokenTypes {
#endif
	enum {
		EOF_ = 1,
		BASE_TAG = 4,
		A_TAG = 5,
		IMG_TAG = 6,
		WORD = 7,
		END_OF_CHUNK = 8,
		PUNCTUATION_MARK = 9,
		TAG_ID = 10,
		ATTR_ID = 11,
		WS = 12,
		ID_LETTER = 13,
		DIGIT = 14,
		ESC = 15,
		FILTER_SYMBOL = 16,
		HREF_NAME = 17,
		SRC_NAME = 18,
		BASE_ATTR_LIST = 19,
		SKIP_ATTR_LIST = 20,
		STRING = 21,
		STYLE_NAME = 22,
		BASE_NAME = 23,
		SCRIPT_NAME = 24,
		IMG_NAME = 25,
		A_NAME = 26,
		PRE_NAME = 27,
		DOCTYPE_NAME = 28,
		TAG = 29,
		DOCTYPE_CONTENT = 30,
		IMG_ATTR_LIST = 31,
		STYLE_CONTENT = 32,
		SCRIPT_CONTENT = 33,
		COMMENT_CONTENT = 34,
		A_ATTR_LIST = 35,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
#endif /*INC_HTMLLexerTokenTypes_hpp_*/
