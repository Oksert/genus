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
#include "Deserializer.h"
#include "Serializer.h"

namespace Similarity
{
	class DocumentStatistics : public Serializable
	{
	private:
		real *statistics;
		size_t identifiersNumber;
	protected:
		DocumentStatistics(Deserializer &deser);
	public:
		size_t getMaxIdentifierNumber();
		/**
		 * Serialization format:
		 * [identifiersNumber] [statistics array]
		 */
		virtual void serialize(Serializer &ser);
		DocumentStatistics(size_t idsNum);
		real &operator [](size_t index);
		virtual ~DocumentStatistics();
	};
}
