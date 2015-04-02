/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/17

*******************************************************************************/

#include "SparseDocumentCollection.h"

using namespace Similarity;

size_t SparseDocumentCollection::getMaxIndex()
{
	return maxIndex;
}

void SparseDocumentCollection::addDocument(Similarity::SparseDocument *doc)
{
	if(maxIndex < doc->getMaxIndex())
	{
		maxIndex = doc->getMaxIndex();
	}
	documents.push_back(doc);
}

void SparseDocumentCollection::mergeIn(Similarity::SparseDocumentCollection &collection)
{
	for(size_t i = 0; i < collection.size(); i++)
	{
		addDocument(&collection[i]);
	}
	collection.documents.clear();
}

SparseDocument &SparseDocumentCollection::operator [](size_t index)
{
	return *documents[index];
}

size_t SparseDocumentCollection::size()
{
	return documents.size();
}

void SparseDocumentCollection::serialize(Serializer &ser)
{
	ser.write((int)documents.size());
	for(size_t i = 0; i < documents.size(); i++)
	{
		documents[i]->serialize(ser);
	}
}

SparseDocumentCollection::SparseDocumentCollection(Similarity::Deserializer &deser)
{
	std::cout << "SparseDocumentCollection1" << std::endl;
	size_t docNum = deser.readInt();
	std::cout << "docNum = " << docNum << std::endl;
	for(size_t i = 0; i < docNum; i++)
	{
		SparseDocument *newDoc = new SparseDocument(deser);
		//std::cout << "SparseDocumentCollection " <<i<< std::endl;
		addDocument(newDoc);
	}
}

SparseDocumentCollection::SparseDocumentCollection()
{
	maxIndex = 0;
}

SparseDocumentCollection::~SparseDocumentCollection()
{
	for(size_t i = 0; i < documents.size(); i++)
	{
		delete documents[i];
	}
	documents.clear();
}


/*--------------------------- AP adapters -----------------------------------*/
/*
SparseDocumentCollection &SparseDocumentCollection::operator =(const Similarity::SparseDocumentCollection &other)
{
	// clear old data
	for(size_t i = 0; i < documents.size(); i++)
	{
		delete documents[i];
	}
	documents.clear();

	// copy new data
	maxIndex = other.maxIndex;
	for(size_t i = 0; i < other.documents.size(); i++)
	{
		documents.push_back(new SparseDocument(*other.documents[i]));
	}

	return *this;
}*/


void SparseDocumentCollection::setMaxIndex(size_t ind)
{
	maxIndex = ind;
}
