#ifndef CONTEXT_H_
#define CONTEXT_H_

class Context
{
private:
	int contextId;
	Context(int id) :contextId(id) {};
public:
	Context(const Context &c) {contextId = c.contextId;};
	Context &operator = (const Context &c)
	{
		contextId = c.contextId;
		return *this;
	}
	std::string operator () (std::string filename);
	static Context getBaseContext();
	static Context getThreadContext();
};


#endif /*CONTEXT_H_*/
