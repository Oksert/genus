#pragma once

class SimpleByteStream : public ByteStream
{
private:
	std::istream &in;
public:
	virtual int get()
	{
#ifdef BENCHMARK
		byteCount++;
#endif
		return in.get();
	};
	SimpleByteStream(std::istream &input)
	: in(input)
	{};
	virtual ~SimpleByteStream()
	{};
};
