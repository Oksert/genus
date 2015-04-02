#ifndef LOGGER_H_
#define LOGGER_H_

class PyLogger
{
private:
	enum LogType
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		CRITICAL
	};
	LogType type;
	std::string buffer;
	std::string path;
	PyLogger(std::string pth, LogType _type) {path = pth;type = _type;};
public:
	static PyLogger debug(std::string str = "", std::string path = "");
	static PyLogger info(std::string str = "", std::string path = "");
	static PyLogger warning(std::string str = "", std::string path = "");
	static PyLogger error(std::string str = "", std::string path = "");
	static PyLogger critical(std::string str = "", std::string path = "");
	PyLogger &operator << (std::string str);
	PyLogger &operator << (int i);
	PyLogger &operator << (float f);
	~PyLogger();
};

#endif /*LOGGER_H_*/
