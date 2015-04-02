// Parser.cpp : Defines the entry point for the console application.
//

#include "ParserStartup.h"
#include <libstemmer.h>
#include "HTMLParserIncludes.h"
#include "StreamClass.h"
#include "HTMLParser.h"
#include "DictionaryIncludes.h"
#include "Grammator.h"
#include "Stemmer.h"
#include "StopWordsFilter.h"
#include "NumericFilter.h"
#include <Context.h>
#include <Logger.h>
#include "ParserLibrary.h"
#include "OutputGenerator.h"
#include <typeinfo>
#include <stdlib.h>
#include <fstream>
//#include <sys/time.h>

#define LOG_PATH "Parser"

#define PUNCT_VALUE (0)
using namespace std;
using namespace HTMLParserLibrary;

void HTMLParserLibrary::Parse(char *text, HTMLParserLibrary::Parameters *param, OutputGenerator *gen)
{
	try
	{
//struct timespec startTime1; 
//clock_gettime(CLOCK_REALTIME, &startTime1);
		
		PyLogger::info("",LOG_PATH) << "Parsing with parameters:";
		PyLogger::info("",LOG_PATH) << "Train: " << int(param->train);
		PyLogger::info("",LOG_PATH) << "Processing (0 - None, 1 - Stemming, 2 - gramm): " << int(param->procType);
		PyLogger::info("",LOG_PATH) << "Gramm number: " << param->grammNumber;
		PyLogger::info("",LOG_PATH) << "Convert encodings: " << int(param->conv);
		if(param->enc_name)
			PyLogger::info("",LOG_PATH) << "Encoding: " << param->enc_name;
		PyLogger::debug("",LOG_PATH) << "Starting parser";
		PyLogger::debug("",LOG_PATH) << "Getting contexts";
		Context context = Context::getThreadContext();
		Context base_context = Context::getBaseContext();
		PyLogger::debug("",LOG_PATH) << "Initializing case converter and stop-words database.";
		HTMLParserLibrary::UTFCaseChanger changer(0x10500,base_context("CaseFolding.case").c_str());
	    DictionaryDB::Dictionary stopWords(base_context("StopWords.db"));
	    DictionaryDB::DictionaryInterface *dict;
	    
	    PyLogger::debug("",LOG_PATH) << "Initializing dictionary.";
	    dict = new DictionaryDB::Dictionary(context("Dictionary.db"));
	    
	    ByteStream *byteStream;
	    HTMLParserLibrary::HTMLParser *parser;
	    Grammator *gramm;
	    RuEnStemmer *stem;
	    
	    StreamClass *output;

		PyLogger::debug("",LOG_PATH) << "Creating parsing pipe.";
	    if(param->conv == false) 
			byteStream = new BufferByteStream(text);
	    else
	    {
	    	HTMLParserLibrary::ConvertedStream *convStr = new HTMLParserLibrary::ConvertedStream();
	    	convStr->set_encoding(param->enc_name);
	    	convStr->set_input(new BufferByteStream(text));
	    	byteStream = convStr;
	    }
	    
	    parser = new HTMLParserLibrary::HTMLParser(changer);
	    parser->SetStream(byteStream);
	    NumericFilter *num_filter = new NumericFilter(parser);
	    StopWordsFilter *filter = new StopWordsFilter(num_filter,stopWords);
	    
	    if(param->procType == PROCESSING_NONE)
	    {
	    	output = filter;
	    }
	    else if(param->procType == PROCESSING_GRAMM)
	    {
	    	gramm = new Grammator(param->grammNumber);
	    	gramm->SetStream(filter);
	    	output = gramm;
	    }
	    else // PROCESSING_STEMMING
	    {
	    	stem = new RuEnStemmer(filter);
	    	output = stem;
	    }
	    //std::cout << output->GetLexem() << std::endl;
	    //std::cout << typeid(output).name() << std::endl;
	    unsigned int bytesRead;
	    unsigned int startTime = clock();
	    
	    PyLogger::debug("",LOG_PATH) << "Processing text...";
	    ofstream f;
	    f.open("123.txt");
	    while(true)
	    {
	    	try
	    	{
	    		std::string curLex;
	    		curLex = output->GetLexem();
	    		
	    		f << curLex + "\n";
	    		if(param->train)
		    	{
	    			size_t id = dict->Write(curLex,1);
		    		gen->PutValue(id);
		    	}
	    		else if(dict->Exists(curLex))
	    		{
	    			size_t id = dict->GetID(curLex);
	    			gen->PutValue(id);
	    		}
	    		else
	    			continue;
	    	}
	    	catch(EndOfStreamException &e)
	    	{
	    		break;
	    	}
	    	catch(AException &e)
	    	{
	    		PyLogger::debug("",LOG_PATH) << "Encountered link: " << e.getLink();
	    		gen->PutLink(e.getLink().c_str());
	    		PyLogger::debug("",LOG_PATH) << "Link stored";
	    	}
	    	catch(IMGException &e)
	    	{
	    		PyLogger::debug("",LOG_PATH) << "Encountered image: " << e.getLink();
	    		// ignore for now
	    	}
	    	catch(BASEException &e)
	    	{
	    		PyLogger::debug("",LOG_PATH) << "Encountered base link: " << e.getLink();
	    		gen->SetBaseUrl(e.getLink().c_str());
	    	}
	    	catch(PunctException &e)
	    	{
	    		if(param->extractPunct)
	    			gen->PutValue(PUNCT_VALUE);
	    	}
			catch(EndOfChunkException &e)
			{
				if(param->multiBlockPackage)
				{
					PyLogger::debug("",LOG_PATH) << "Finished processing chunk.";
					// select next output generator.
					gen++;
				}
				else
				{
					// ignore the end-of-chunk tag...
				}
			}
	    }
	    PyLogger::debug("",LOG_PATH) << "Processing finished.";
	    delete dict;

//struct timespec endTime1; 
//clock_gettime(CLOCK_REALTIME, &endTime1);
//std::cout<<"ParsingTime=" << endTime1.tv_sec-startTime1.tv_sec<<" "<<endTime1.tv_nsec-startTime1.tv_nsec<<std::endl;
//int t1 = endTime1.tv_sec-startTime1.tv_sec;
//int t2 = endTime1.tv_nsec-startTime1.tv_nsec;

////PyLogger::debug("","pyBinding.ParsingTime=") << t1;
////PyLogger::debug("","pyBinding.ParsingTime=") << t2;
////PyLogger::info("",LOG_PATH) << "ParsingTime="<<t1<<" "<<t2;
	}
	catch(...)
	{
		PyLogger::error("",LOG_PATH) << "Unknown error occured.";
	}
}

