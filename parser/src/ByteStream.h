#pragma once

class ByteStream
{
protected:

#ifdef BENCHMARK
	unsigned int byteCount;
#endif

public:
	/**
	 * Get the next byte from the stream
	 */
	virtual int get() = 0;
	
#ifdef BENCHMARK
	unsigned int GetBytesProcessed()
	{
		return byteCount;
	};
	ByteStream() { byteCount = 0; };
#endif

	virtual ~ByteStream() {};
};
