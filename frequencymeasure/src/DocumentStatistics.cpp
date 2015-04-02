/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/17

*******************************************************************************/

#include "DocumentStatistics.h"

using namespace Similarity;

DocumentStatistics::DocumentStatistics(size_t idsNum)
{
	identifiersNumber = idsNum;
	statistics = new real[identifiersNumber+1];
	for(size_t i = 0; i <= identifiersNumber; i++)
	{
		statistics[i] = 0;
	}
}

DocumentStatistics::DocumentStatistics(Deserializer &deser)
{
	identifiersNumber = deser.readInt();
	statistics = new real[identifiersNumber + 1];
	deser.read(statistics,(unsigned int)(sizeof(real)*(identifiersNumber+1)));
}

DocumentStatistics::~DocumentStatistics()
{
	delete statistics;
}

real &DocumentStatistics::operator [](size_t index)
{
	return statistics[index];
}

void DocumentStatistics::serialize(Similarity::Serializer &ser)
{
	ser.write((int)identifiersNumber);
	ser.write(statistics,(unsigned int)(sizeof(real)*identifiersNumber));
}

size_t DocumentStatistics::getMaxIdentifierNumber()
{
	return identifiersNumber;
}
