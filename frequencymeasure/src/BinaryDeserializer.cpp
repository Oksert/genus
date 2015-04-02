/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/17

*******************************************************************************/

#include "BinaryDeserializer.h"

using namespace Similarity;

BinaryDeserializer::BinaryDeserializer(const char *filename)
{
	file = fopen(filename,"rb");
	if (file==NULL)
	{
		std::cout << "ERROR open file" << filename << std::endl;
	}

}

BinaryDeserializer::~BinaryDeserializer()
{
	fclose(file);
}

void BinaryDeserializer::read(void *ptr, unsigned int size)
{
	fread(ptr,1,size,file);
}

double BinaryDeserializer::readDouble()
{
	double res;
	read(&res,sizeof(double));
	return res;
}

float BinaryDeserializer::readFloat()
{
	float res;
	read(&res,sizeof(float));
	return res;
}

int BinaryDeserializer::readInt()
{
	int res;
	read(&res,sizeof(int));
	return res;
}

std::string BinaryDeserializer::readString()
{
	int size = readInt();
	char *buf = new char[size+1];
	for(int i = 0; i < size + 1; i++)
	{
		buf[i] = 0;
	}
	read(buf,size);
	std::string res(buf);
	delete buf;
	return res;
}
