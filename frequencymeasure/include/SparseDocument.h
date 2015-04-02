/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/12
    Description: 

*******************************************************************************/

/**
 * @file
 * 
 */

#pragma once

#include "Types.h"
#include "AlternativeAllocator.h"
#include "Serializable.h"
#include "Document.h"
#include "Deserializer.h"
#include "IDFStatistics.h"

namespace Similarity
{
	class IDFStatistics;

	struct Element : public AltAllocated<Element>
	{
		Element *next;
		size_t index;
		real weight;
		Element(size_t ind = 0, real w = 0.0);
	};

	class SparseDocument : public Serializable
	{
	private:
		

		Element list;
		size_t maxLocalIndex;
	protected:
	public:
		SparseDocument(size_t index, real weight);
		void mergeIn(SparseDocument &other);

		/**
		 * Serialization format:
		 * {[index] [weight]}* [-1]
		 */
		virtual void serialize(Serializer &ser);

		/**
		 * Main interface.
		 */
		SparseDocument();
		SparseDocument(Document &doc);
		SparseDocument(Deserializer &deser);
		SparseDocument &operator += (const SparseDocument &other);
		SparseDocument &operator /= (const real val);
		SparseDocument &operator =(const SparseDocument &other);
		SparseDocument(const SparseDocument &other);

		void normalize();
		void adjustBy(IDFStatistics &stats);
		real dot(const SparseDocument &other) const;
		real dist(const SparseDocument &other) const;
		size_t getMaxIndex();

		virtual ~SparseDocument();
		friend class DFStatistics;
		friend class DenseDocument;
	};

}

// ap functions adapters
