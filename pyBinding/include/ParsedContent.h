#ifndef PARSEDCONTENT_
#define PARSEDCONTENT_

namespace CModule
{

/**
 * Identifier type
 */
typedef unsigned int id;

class ParsedContent
{
public:
	/**
	 * Returns list of links, contained in the document.
	 */
	virtual std::vector<std::string> getLinks()
	{
		throw std::exception();
	}
	/**
	 * Replaces the link with it's topic identifiers.
	 */
	virtual void replaceLink(std::string link, std::vector<std::string> ids)
	{
		throw std::exception();
	}
	/**
	 * Returns the resulting list of identifiers, with links replace.
	 */
	virtual std::vector<id> getIds()
	{
		throw std::exception();
	}
	/**
	 * Returns the initial list of identifiers, without links replaced.
	 */
	virtual std::vector<id> getWordsIds()
	{
		throw std::exception();
	}
	/**
	 * Returns base url of the document
	 */
	 virtual std::string getBaseUrl()
	 {
	 	throw std::exception();
	 }
	 virtual void writeIds()
	{
		throw std::exception();
	}
	 virtual void writeTopicsIntoDB(std::vector<CModule::id> topics)
	{
		throw std::exception();
	}
};

}

#endif /*PARSEDCONTENT_*/
