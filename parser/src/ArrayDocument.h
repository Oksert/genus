#ifndef ARRAYDOCUMENT_H_
#define ARRAYDOCUMENT_H_

class ArrayDocument : public Similarity::Document
{
private:
	size_t *array;
	size_t readAlready;
	size_t generalLength;
public:
	ArrayDocument(size_t *arr, size_t len)
	{
		array = arr;
		generalLength = len;
		readAlready = 0;
	}
	virtual size_t read(size_t *values, size_t maxRead)
	{
		for(size_t i = 0; i < maxRead; i++)
		{
			if(readAlready >= generalLength)
				return i;
			values[i] = array[readAlready];
			readAlready++;
		}
		return maxRead;
	}
	
	virtual size_t size()
	{
		return generalLength;
	}
};

#endif /*ARRAYDOCUMENT_H_*/
