#pragma once

class Grammator : public StreamClass {
private:
	StreamClass *stream; // input stream
	
	int dimension; // n in n-gramm

	std::list<std::string> cached_gramms;
	
	void cache(std::string lexem);
public:
	std::string GetLexem() throw (StreamException);
	void SetStream(StreamClass* stream_obj); // initializes stream
	Grammator(int n);
	~Grammator();
};
