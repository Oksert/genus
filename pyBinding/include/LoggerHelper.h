#ifndef LOGGERHELPER_H_
#define LOGGERHELPER_H_

struct LoggerHelper
{
	static object getLogger;
	static void setLoggers(PyObject * getL)
	{
		getLogger = object(handle<>(getL));
	}
};

#endif /*LOGGERHELPER_H_*/
