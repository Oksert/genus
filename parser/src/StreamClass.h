#ifndef __STREAMCLASS_H_
#define __STREAMCLASS_H_

#ifndef PARSER_LIBRARY_STARTUP
#error Include ParserStartup.h before including any other headers.
#endif 

class StreamException
{};

class EndOfStreamException : public StreamException
{};

class EndOfChunkException : public StreamException
{};

class AException : public StreamException
{
private:
	std::string link;
public:
	std::string getLink() {return link;};
	AException(std::string s) :link(s) {};
};

class BASEException : public StreamException
{
private:
	std::string link;
public:
	std::string getLink() {return link;};
	BASEException(std::string s) :link(s) {};
};

class IMGException : public StreamException
{
private:
	std::string link;
public:
	std::string getLink() {return link;};
	IMGException(std::string s) :link(s) {};
};

class PunctException : public StreamException
{
private:
	std::string mark;
public:
	std::string getMark() {return mark;};
	PunctException(std::string s) :mark(s) {};
};

class StreamClass {
protected:
public:
    virtual std::string GetLexem() throw (StreamException) = 0; 
};


#endif

