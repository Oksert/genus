/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/17
    Description: 

*******************************************************************************/

/**
 * @file
 * 
 */

#pragma once

#include "Types.h"

namespace Similarity
{
	/**
	 * The abstract document interface.
	 * Specifies the way document contents are read by the SimilarityMeasure
	 * instances.
	 */
	class Document
	{
	private:
	protected:
	public:
		/**
		 * Get the next batch of the document data.
		 *
		 * @param values the destination array, where to put read values
		 *
		 * @param maxRead maximum number of values to read
		 *
		 * @return the number of values written to the destination array.
		 */
		virtual size_t read(size_t *values, size_t maxRead) = 0;

		/**
		 * Returns the size of the document.
		 */
		virtual size_t size() = 0;

		virtual ~Document() {};
	};
}
