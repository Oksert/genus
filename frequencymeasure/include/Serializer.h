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
	class Serializer
	{
	private:
	protected:
	public:
		virtual void write(int i) = 0;
		virtual void write(float f) = 0;
		virtual void write(double d) = 0;
		virtual void write(std::string s) = 0;
		virtual void write(void *ptr, unsigned int size) = 0;
		virtual ~Serializer();
	};
}
