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
#include "Deserializer.h"

namespace Similarity
{
	class SparseDocumentCollection;

	class DFStatistics : public DocumentStatistics
	{
	private:
		size_t documentsNumber;
		void collect(SparseDocumentCollection &collection);
	protected:
	public:
		/**
		 * Serialization format:
		 * [super] [documentsNumber]
		 */
		virtual void serialize(Serializer &ser);
		DFStatistics(Deserializer &deser);
		DFStatistics(SparseDocumentCollection &collection);
		DFStatistics(SparseDocumentCollection &collection, size_t maxIndex);
		void addStatistics(DFStatistics &otherStats);
		size_t getDocumentsNumber();
		virtual ~DFStatistics();
	};
}
