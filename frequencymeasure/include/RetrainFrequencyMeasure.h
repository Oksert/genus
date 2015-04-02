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
#include "RetrainSimilarityMeasure.h"
#include "SparseDocumentCollection.h"
#include "DFStatistics.h"
#include "IDFStatistics.h"

namespace Similarity
{
	class RetrainFrequencyMeasure : public RetrainSimilarityMeasure
	{
	protected:
		// raw data - generated during preload, modified during retrain
		SparseDocumentCollection *nonadjustedDocuments;
		DFStatistics *frequencies;
		bool rawDataLoaded;

		// adjusted data - generated during preload and retrain, used in similarity functions
		SparseDocumentCollection *adjustedDocuments;
		IDFStatistics *idf;
		bool adjustedDataLoaded;

		std::string pathString;

		virtual void clean();

		virtual void loadRawData();
		virtual void loadAdjustedData();

		virtual void saveRawData();
		virtual void saveAdjustedData();
	public:

		virtual void retrain(Document **docs, size_t number);

		virtual real similarity(Document &doc1, Document &doc2);
		virtual void similarity(Document &doc, real *dest);
		virtual real similarity(size_t i1, size_t i2);
		virtual void similarity(size_t i, real *dest);
		virtual size_t preloadedSetSize();
		virtual void preload(Document **docs, size_t number);
		virtual void saveToBinaryFile(const char *preloadedSetFilename);
		virtual void readFromBinaryFile(const char *preloadedSetFilename);
		RetrainFrequencyMeasure();
		virtual ~RetrainFrequencyMeasure();
	};
}
