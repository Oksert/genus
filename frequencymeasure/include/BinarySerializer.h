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
#include "Serializer.h"

namespace Similarity
{

	class BinarySerializer : public Serializer
	{
	private:
		FILE *file;
	protected:
	public:
		virtual void write(int i);
		virtual void write(float f);
		virtual void write(double d);
		virtual void write(std::string s);
		virtual void write(void *ptr, unsigned int size);
		BinarySerializer(const char *filename);
		virtual ~BinarySerializer();
	};
}
