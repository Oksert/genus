/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/17

*******************************************************************************/

#include "DFStatistics.h"

using namespace Similarity;

DFStatistics::DFStatistics(Deserializer &deser)
:DocumentStatistics(deser)
{
	documentsNumber = deser.readInt();
}

DFStatistics::~DFStatistics()
{}

DFStatistics::DFStatistics(SparseDocumentCollection &collection, size_t maxIndex)
:DocumentStatistics(maxIndex)
{
	collect(collection);
}

DFStatistics::DFStatistics(SparseDocumentCollection &collection)
:DocumentStatistics(collection.getMaxIndex())
{
	collect(collection);
}

void DFStatistics::serialize(Serializer &ser)
{
	DocumentStatistics::serialize(ser);
	ser.write((int)documentsNumber);
}

void DFStatistics::collect(Similarity::SparseDocumentCollection &collection)
{
	std::cout << "DFStatistics::collect1" << std::endl;
	documentsNumber = collection.size();
	std::cout << "documentsNumber" << documentsNumber<< std::endl;
	for(size_t i = 0; i < documentsNumber; i++)
	{
		Element *list = collection[i].list.next;
		while(list != NULL)
		{
			// the document contains an element - save this information
			(*this)[list->index] += 1.0;
			list = list->next;
		}
	}
	std::cout << "DFStatistics::collect1" << std::endl;
}

size_t DFStatistics::getDocumentsNumber()
{
	return documentsNumber;
}

void DFStatistics::addStatistics(Similarity::DFStatistics &otherStats)
{
	for(size_t i = 0; i <= getMaxIdentifierNumber(); i++)
	{
		(*this)[i] += otherStats[i];
	}
}
