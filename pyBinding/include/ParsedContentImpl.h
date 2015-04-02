#ifndef PARSEDCONTENTIMPL_
#define PARSEDCONTENTIMPL_


#include "DictionaryIncludes.h"
#include <Context.h>
#include <sstream>

int count1=1;
int count2=1;


class ParsedContentImpl : public CModule::ParsedContent
{
private:
	HTMLParserLibrary::OutputGenerator *generator;
	std::map<std::string,CModule::id> topicsMap;
public:
	std::vector<std::string> getLinks()
	{
		std::vector<std::string> ret;
		HTMLParserLibrary::OutputGenerator::LinkInfoList *ptr = generator->linkList.next;
		while(ptr != NULL)
		{
			ret.push_back(std::string(ptr->url));
			ptr = ptr->next;
		}
		return ret;
	}
	void replaceLink(std::string link, std::vector<std::string> ids)
	{
		HTMLParserLibrary::OutputGenerator::LinkInfoList *ptr = generator->linkList.next;
		int id = 0;
		std::ofstream fout("replaceLink.txt");

		Context context = Context::getThreadContext();
		DictionaryDB::DictionaryInterface *dict = new DictionaryDB::Dictionary(context("Dictionary.db"));

		while(ptr != NULL)
		{
			if(link == std::string(ptr->url))
			{
				//std::cout << "New Link: ";
				ptr->linkDataSize = ids.size();
				ptr->linkData = new size_t[ptr->linkDataSize];
				for(size_t i = 0; i < ptr->linkDataSize; i++)
				{
					if (topicsMap.find(ids[i])==topicsMap.end()) {
						if(dict->Exists(ids[i])) topicsMap[ids[i]] = dict->GetID(ids[i]);
						else topicsMap[ids[i]] = dict->Write(ids[i],1);
					}
					ptr->linkData[i] = topicsMap.find(ids[i])->second;
					fout << ptr->linkData[i] << std::endl;
					//std::cout << topicsMap.find(ids[i])->second << " " << ids[i];
				}
				//std::cout << std::endl;
			}
			ptr = ptr->next;
		}
		
		delete dict;
	}
	std::vector<CModule::id> getIds()
	{
		// TODO Add link replacement here.
		std::vector<CModule::id> ret;
		HTMLParserLibrary::OutputGenerator::BufferList *ptr = generator->list.nextBuffer;

		HTMLParserLibrary::OutputGenerator::LinkInfoList *link_ptr = generator->linkList.next;
		int pos = 0;
		int c1=0;
		int c2=0;
		int linkPos;
		if (link_ptr != NULL) linkPos = link_ptr->position;
		else linkPos = -1;
		
		while(ptr != NULL)
		{
			for(size_t i = 0; i < ptr->usedNumber; i++)
			{
				while (pos==linkPos-1)
				{
					if (link_ptr->linkDataSize!=0)
					{
						c1++;
						ret.insert(ret.end(),link_ptr->linkData,link_ptr->linkData+link_ptr->linkDataSize);
					}
					link_ptr=link_ptr->next;
					if (link_ptr == NULL) break;
					linkPos = link_ptr->position;
				}
				c2++;
				ret.push_back(ptr->buffer[i]);
				pos++;
			}
			ptr = ptr->nextBuffer;
		}
		return ret;
	}
	std::vector<CModule::id> getWordsIds()
	{
		std::vector<CModule::id> ret;
		HTMLParserLibrary::OutputGenerator::BufferList *ptr = generator->list.nextBuffer;
		while(ptr != NULL)
		{
			for(size_t i = 0; i < ptr->usedNumber; i++)
			{
				ret.push_back(ptr->buffer[i]);
			}
			ptr = ptr->nextBuffer;
		}
		return ret;
	}
	ParsedContentImpl(HTMLParserLibrary::OutputGenerator *gen)
	{
		cout <<"1";
		generator = gen;
		cout <<"2";
	}
	~ParsedContentImpl()
	{
		if(generator) delete generator;
	}
	std::string getBaseUrl()
	{
		char *r = generator->GetBaseUrl(); 
		if (r==NULL) return std::string("");
		else return std::string(generator->GetBaseUrl());
	}
	void writeIds()
	{
		std::ofstream fout0("tmp0.txt",ios::app);
		int pos=0;
		HTMLParserLibrary::OutputGenerator::BufferList * ptr = generator->list.nextBuffer;
		while (ptr!=NULL)
		{
			for (size_t i=0;i<ptr->usedNumber;i++)
			{
				//write entry
				fout0 << pos << ": " << ptr->buffer[i] << std::endl;
				pos++;
			}
			ptr=ptr->nextBuffer;
		}
		fout0 << count2 << "/////////////////////////////////////" << std::endl;
		count2++;
	}
};

#endif /*PARSEDCONTENTIMPL_*/
