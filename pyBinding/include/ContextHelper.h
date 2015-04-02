#ifndef CONTEXTHELPER_H_
#define CONTEXTHELPER_H_

struct StaticObjectHolder
{
	static object contextManager;
	static void setContextManager(PyObject * cm) {contextManager = object(handle<>(cm));};
};

#endif /*CONTEXTHELPER_H_*/
