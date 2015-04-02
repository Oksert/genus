#include <boost/python.hpp>
#include <string>
#include "Logger.h"

//#include <boost/python.hpp>
using namespace boost::python;
#include "LoggerHelper.h"

object LoggerHelper::getLogger;

PyLogger PyLogger::debug(std::string str, std::string path)
{
	PyLogger logger(path,DEBUG);
	logger.buffer = str;
	return logger;
}

PyLogger PyLogger::info(std::string str, std::string path)
{
	PyLogger logger(path,INFO);
	logger.buffer = str;
	return logger;
}

PyLogger PyLogger::warning(std::string str, std::string path)
{
	PyLogger logger(path,WARNING);
	logger.buffer = str;
	return logger;
}

PyLogger PyLogger::error(std::string str, std::string path)
{
	PyLogger logger(path,ERROR);
	logger.buffer = str;
	return logger;
}

PyLogger PyLogger::critical(std::string str, std::string path)
{
	PyLogger logger(path,CRITICAL);
	logger.buffer = str;
	return logger;
}

PyLogger &PyLogger::operator << (std::string str)
{
	buffer += str;
	return *this;
}

PyLogger &PyLogger::operator << (float f)
{
	char buf[20] = {0};
	sprintf(buf,"%f",f);
	buffer += buf;
	return *this;
}

PyLogger &PyLogger::operator << (int i)
{
	char buf[20] = {0};
	sprintf(buf,"%i",i);
	buffer += buf;
	return *this;
}

PyLogger::~PyLogger()
{
	if(buffer == "")
		return;
	// write the results
	switch(type)
	{
	case DEBUG:
		LoggerHelper::getLogger(path).attr("debug")(buffer);
		break;
	case INFO:
		LoggerHelper::getLogger(path).attr("info")(buffer);
		break;
	case WARNING:
		LoggerHelper::getLogger(path).attr("warning")(buffer);
		break;
	case ERROR:
		LoggerHelper::getLogger(path).attr("error")(buffer);
		break;
	case CRITICAL:
		LoggerHelper::getLogger(path).attr("critical")(buffer);
		break;
	}
}
