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
#include "Deserializer.h"

namespace Similarity
{
	class BinaryDeserializer : public Deserializer
	{
	private:
		FILE *file;
	protected:
	public:
		virtual int readInt();
		virtual float readFloat();
		virtual double readDouble();
		virtual std::string readString();
		virtual void read(void *ptr, unsigned int size);
		BinaryDeserializer(const char *filename);
		virtual ~BinaryDeserializer();
	};
}
