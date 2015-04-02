#include "ParserStartup.h"
#include <string.h>
#include "OutputGenerator.h"

using namespace HTMLParserLibrary;

void OutputGenerator::PutValue(size_t val)
{
	if(currentBuffer->nextBuffer == NULL)
		currentBuffer->nextBuffer = new BufferList;
	currentBuffer->nextBuffer->buffer[currentBuffer->nextBuffer->usedNumber] = val;
	currentBuffer->nextBuffer->usedNumber++;
	if(currentBuffer->nextBuffer->usedNumber == BufferList::bufferSize)
	{
		currentBuffer = currentBuffer->nextBuffer;
	}
	curIndex++;
	curSize++;
}

void OutputGenerator::PutLink(const char *url)
{
	if(currentLink->next != NULL)
	{
		size_t prevPos = currentLink->next->position;
		size_t prevNumber = currentLink->next->number;
		currentLink->next->next = new LinkInfoList;
		currentLink = currentLink->next;
		currentLink->next->position = curIndex;
		if(curIndex == prevPos)
		{
			currentLink->next->number = prevNumber + 1;
		}
		else
		{
			currentLink->next->number = 0;
		}
		currentLink->next->url = new char[strlen(url) + 1];
		strcpy(currentLink->next->url,url);
	}
	else
	{
		currentLink->next = new LinkInfoList;
		currentLink->next->position = curIndex;
		currentLink->next->number = 0;
		currentLink->next->url = new char[strlen(url) + 1];
		strcpy(currentLink->next->url,url);
	}
}

size_t OutputGenerator::Size()
{
	return curSize;
}

void OutputGenerator::SetBaseUrl(const char *url)
{
	baseUrl = new char[strlen(url) + 1];
	strcpy(baseUrl,url);
}

char* OutputGenerator::GetBaseUrl() 
{
	return baseUrl;
}	

void OutputGenerator::Clear()
{
	linkList.DeleteRecursively();
	list.DeleteRecursively();
	if(baseUrl)
		delete baseUrl;
	baseUrl = NULL;
	currentBuffer = &list;
	currentLink = &linkList;
	curSize = 0;
	curIndex = 0;
}

OutputGenerator::OutputGenerator()
{
	std::cout <<"01";
	baseUrl = NULL;
	Clear();
	std::cout <<"02";
}

void OutputGenerator::WriteToDest(size_t *dest)
{
	LinkInfoList *links = &linkList;
	BufferList *lst = &list;
	size_t index = 0;
	size_t iterPos = 0;
	
	while(lst->nextBuffer != NULL && index < lst->nextBuffer->usedNumber)
	{
		if(links->next != NULL && links->next->position == iterPos)
		{
			for(size_t i = 0; i < links->next->linkDataSize; i++)
			{
				*dest = links->next->linkData[i];
				dest++;
			}
			links = links->next;
			continue;
		} 
		*dest = lst->nextBuffer->buffer[index];
		dest++;
		
		index++;
		iterPos++;
		if(index == lst->nextBuffer->usedNumber)
		{
			index = 0;
			lst = lst->nextBuffer;
		}
	}
}

void OutputGenerator::PrintToCout()
{
	if(baseUrl)
		std::cout << baseUrl << std::endl;
	LinkInfoList *links = &linkList;
	BufferList *lst = &list;
	size_t index = 0;
	size_t iterPos = 0;
	
	while(lst->nextBuffer != NULL && index < lst->nextBuffer->usedNumber)
	{
		if(links->next != NULL && links->next->position == iterPos)
		{
			for(size_t i = 0; i < links->next->linkDataSize; i++)
			{
				std::cout << links->next->linkData[i] << ", ";
			}
			links = links->next;
			continue;
		} 
		std::cout << lst->nextBuffer->buffer[index] << ", ";
		
		index++;
		iterPos++;
		if(index == lst->nextBuffer->usedNumber)
		{
			index = 0;
			lst = lst->nextBuffer;
		}
	}
	std::cout << 0 << std::endl << "end of sequence" << std::endl;
}

OutputGenerator::~OutputGenerator()
{
	Clear();
}
