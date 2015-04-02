#pragma once

ANTLR_BEGIN_NAMESPACE(HTMLParserLibrary)
/* $ANTLR 2.7.6 (2005-12-22): "conversion.g" -> "ConversionParserTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API ConversionParserTokenTypes {
#endif
	enum {
		EOF_ = 1,
		NUMBER = 4,
		C_TYPE = 5,
		S_TYPE = 6,
		T_TYPE = 7,
		F_TYPE = 8,
		WS = 9,
		COMMENT = 10,
		HEX_DIGIT = 11,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
