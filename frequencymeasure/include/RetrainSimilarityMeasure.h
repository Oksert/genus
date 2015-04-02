/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/18
    Description: 

*******************************************************************************/

/**
 * @file
 * 
 */

#pragma once

#include "SimilarityMeasure.h"

namespace Similarity
{
	class RetrainSimilarityMeasure : public SimilarityMeasure
	{
	public:
		virtual void retrain(Document **docs, size_t number) = 0;
	};
}
