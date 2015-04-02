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
#include "Serializable.h"
#include "SparseDocument.h"
#include "Serializer.h"
#include "Deserializer.h"

namespace Similarity
{
	class SparseDocument;

	class SparseDocumentCollection : public Serializable
	{
	private:
		std::vector<SparseDocument*> documents;
		size_t maxIndex;
	protected:
	public:
		/**
		 * Serialization format:
		 * [docsNumber] {[SparseDocument]}*
		 */
		virtual void serialize(Serializer &ser);
		SparseDocumentCollection(Deserializer &deser);
		SparseDocumentCollection();
		void mergeIn(SparseDocumentCollection &collection);
		void addDocument(SparseDocument *doc);
		size_t size();
		SparseDocument &operator [](size_t index);
		size_t getMaxIndex();
		void setMaxIndex(size_t ind);
		virtual ~SparseDocumentCollection();

		
	};
}
