/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/17

*******************************************************************************/

#include "IDFStatistics.h"

using namespace Similarity;

IDFStatistics::IDFStatistics(Deserializer &deser)
:DocumentStatistics(deser)
{
	documentsNumber = deser.readInt();
}

IDFStatistics::~IDFStatistics()
{}

void IDFStatistics::serialize(Serializer &ser)
{
	DocumentStatistics::serialize(ser);
	ser.write((int)documentsNumber);
}

IDFStatistics::IDFStatistics(Similarity::DFStatistics &stats)
:DocumentStatistics(stats.getMaxIdentifierNumber())
{
	documentsNumber = stats.getDocumentsNumber();
	for(size_t i = 0; i <= stats.getMaxIdentifierNumber(); i++)
	{
		if(stats[i] > 0.0)
			(*this)[i] = log((real)documentsNumber / stats[i]);
		else
			(*this)[i] = 0.0;
	}
}
