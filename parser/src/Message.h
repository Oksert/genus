#ifndef MESSAGE_H_
#define MESSAGE_H_

class Message
{
private:
	std::string MessageString;
public:
	const char *toString()
	{
		return MessageString.c_str();
	};
	Message(std::string msg)
	{
		MessageString = msg;
	};
};

#endif /*MESSAGE_H_*/
