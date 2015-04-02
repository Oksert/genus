#ifndef PACKAGEPARSER_
#define PACKAGEPARSER_

namespace CModule
{
	class PackageParser
	{
	private:
		std::string encoding;
		HTMLParserLibrary::Parameters params;
		
		std::string stringToParse;
		int chunksNumber;
		
		HTMLParserLibrary::OutputGenerator *generators;
		bool parsed;
		bool firstDoc;
		
		static const char CHUNK_DELIMITER = char(0x1a);
	protected:
	public:

		void addChunk(std::string chunk)
		{
			if(parsed)
				return;
			
			
			if(!firstDoc)
				stringToParse += CHUNK_DELIMITER;
			else
				firstDoc = false;
			
			stringToParse += chunk;
			
			chunksNumber++;
		}

		void parse()
		{
			parsed = true;
			
			if(chunksNumber == 0)
				return;
			
			cout <<"Starting package parse."<<endl;
			PyLogger::debug("","pyBinding.Parser") << "Starting package parse.";
			
			// generate outputGenerators.
			generators = new HTMLParserLibrary::OutputGenerator[chunksNumber];
			
			// finalize parameters.
			params.enc_name = encoding.c_str();
			params.conv = true;
			params.multiBlockPackage = true;
			
			// parse
			HTMLParserLibrary::Parse(const_cast<char*>(stringToParse.c_str()), &params, generators);
			PyLogger::debug("","pyBinding.Parser") << "Package parsing finished";
		}
		
		CModule::ParsedContent *getResult(int index)
		{
			if(index < chunksNumber && index >= 0)
				return new ParsedContentImpl(&generators[index]);
			else
				return NULL;
		}
		
		PackageParser(HTMLParserLibrary::Parameters p, std::string enc)
		{
			params = p;
			encoding = enc;
			parsed = false;
			firstDoc = true;
			chunksNumber = 0;
			stringToParse = "";
			generators = NULL;
		}
		
		~PackageParser()
		{
			PyLogger::debug("","pyBinding.Parser") << "Deleting package parser.";
			if(generators)
				delete []generators;
			PyLogger::debug("","pyBinding.Parser") << "Package parser deleted.";
		}
	};
}

#endif /* PACKAGEPARSER_ */
