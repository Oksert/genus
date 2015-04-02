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

namespace Similarity
{
	class Deserializer
	{
	private:
	protected:
	public:
		virtual int readInt() = 0;
		virtual float readFloat() = 0;
		virtual double readDouble() = 0;
		virtual std::string readString() = 0;
		virtual void read(void *ptr, unsigned int size) = 0;
		virtual ~Deserializer();
	};
}
