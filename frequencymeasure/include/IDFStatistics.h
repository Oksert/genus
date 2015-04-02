/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/16
    Description: 

*******************************************************************************/

/**
 * @file
 * 
 */

#pragma once

#include "Types.h"
#include "DocumentStatistics.h"
#include "SparseDocumentCollection.h"
#include "Serializer.h"
#include "DFStatistics.h"

namespace Similarity
{
	class DFStatistics;

	class IDFStatistics : public DocumentStatistics
	{
	private:
		size_t documentsNumber;
	protected:
	public:
		/**
		 * Serialization format:
		 * [super] [documentsNumber]
		 */
		virtual void serialize(Serializer &ser);
		IDFStatistics(Deserializer &deser);
		IDFStatistics(DFStatistics &stats);
		virtual ~IDFStatistics();
	};
}
