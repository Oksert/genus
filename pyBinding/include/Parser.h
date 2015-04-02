#ifndef PARSER_H_
#define PARSER_H_

namespace CModule
{

class Parser
{
private:
protected:
public:
	/**
	 * Parse the contents of the document, using the specified encoding.
	 * 
	 * @param Content the contents
	 * @param Encoding the string, which corresponds to the encoding, that must be used.
	 * @return a new instance of the ParsedContent, which holds all the data extracted from the document.
	 */
	virtual ParsedContent *parse(std::string Content, std::string Encoding)
	{
		throw std::exception();
	}
	/**
	 * Returns the id of the word in the parser's dictionary.
	 * 
	 * @param lex the word
	 * @return the id of the word
	 */
	virtual id getId(std::string lex)
	{
		throw std::exception();
	}
	/**
	 * Returns the id of the topic in the parser's dictionary.
	 * 
	 * @param topicName the topic
	 * @return the id of the topic
	 */
	virtual id getTopicId(std::string topicName)
	{
		throw std::exception();
	}
	/**
	 * Adds a new topic to the parser's dictionary, and returns it's id.
	 * 
	 * @param topicName the new topic name
	 * @return the id of the topic
	 */
	virtual id addTopic(std::string topicName)
	{
		throw std::exception();
	}
	virtual ~Parser() {};
};

}

#endif /*PARSER_H_*/
