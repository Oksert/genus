#ifndef PARSERBINDING_H_
#define PARSERBINDING_H_

#include <stdlib.h>
//#include <sys/time.h>

class ParserImpl : public CModule::Parser
{
private:
	HTMLParserLibrary::Parameters parameters;
public:
	CModule::ParsedContent *parse(std::string Content, std::string Encoding)
	{
			
		//PyLogger::debug("","pyBinding.Parser") << "Starting parse.";
//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime);
		
		HTMLParserLibrary::OutputGenerator *gen = new HTMLParserLibrary::OutputGenerator();
			
		parameters.enc_name = Encoding.c_str();
		parameters.conv = true;
		HTMLParserLibrary::Parse(const_cast<char*>(Content.c_str()), &parameters, gen);

//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"ParsingTime_pyBinding=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
//int t1 = endTime.tv_sec-startTime.tv_sec;
//int t2 = endTime.tv_nsec-startTime.tv_nsec;
			
		
		return new ParsedContentImpl(gen);

		
//PyLogger::debug("","pyBinding.ParsingTime=") << t1;
//PyLogger::debug("","pyBinding.ParsingTime=") << t2;

	//PyLogger::debug("","pyBinding.Parser") << "Parsing " << "finished";
	}
	CModule::id getId(std::string lex)
	{
		return 0;
	}
	CModule::id getTopicId(std::string topicName)
	{
		return 0;
	}
	CModule::id addTopic(std::string topicName)
	{
		return 0;
	}
	ParserImpl(HTMLParserLibrary::Parameters p) {parameters = p;}
	virtual ~ParserImpl() {};
};

#endif /*PARSERBINDING_H_*/
