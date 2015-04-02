/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/11/01
    Description: 

*******************************************************************************/

#pragma once

#include "StreamClass.h"
# define NO_STATIC_CONSTS
#include <antlr/config.hpp>
#include <antlr/RecognitionException.hpp>
#include <antlr/BitSet.hpp>
#include <antlr/String.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/CommonToken.hpp>
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/LexerSharedInputState.hpp>
#include <antlr/NoViableAltForCharException.hpp>
#include <antlr/NoViableAltException.hpp>
#include <antlr/LLkParser.hpp>

#include "BitSet.h"

#include "ConversionParserTokenTypes.hpp"
#include "HTMLLexerTokenTypes.hpp"

#include "UTFCaseChanger.h"
#include "ByteStream.h"
#include "SimpleByteStream.h"
#include "ConvertedStream.h"

#include "ConversionLexer.hpp"
#include "ConversionParser.hpp"
#include "UnicodeCharBufferConv.h"
#include "UnicodeCharScanner.hpp"
#include "HTMLLexer.hpp"
#include "HTMLParser.h"

#include "OutputGenerator.h"
