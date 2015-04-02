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
#include "Serializer.h"

namespace Similarity
{
	class Serializable
	{
	private:
	protected:
	public:
		virtual void serialize(Serializer &ser) = 0;
		virtual ~Serializable();
	};
}
