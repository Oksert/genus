/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/17

*******************************************************************************/

#include "BinarySerializer.h"

using namespace Similarity;

BinarySerializer::BinarySerializer(const char *filename)
{
	file = fopen(filename,"wb+");
}

BinarySerializer::~BinarySerializer()
{
	fclose(file);
}

void BinarySerializer::write(double d)
{
	write(&d,sizeof(double));
}

void BinarySerializer::write(float d)
{
	write(&d,sizeof(float));
}

void BinarySerializer::write(int d)
{
	write(&d,sizeof(int));
}

void BinarySerializer::write(std::string s)
{
	write((int)s.size());
	write(const_cast<char*>(s.c_str()),(unsigned int)s.size());
}

void BinarySerializer::write(void *ptr, unsigned int size)
{
	fwrite(ptr,1,size,file);
}
