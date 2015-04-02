#include <boost/python.hpp>

#include <string>
#include "Context.h"


using namespace boost::python;

#include "ContextHelper.h"

object StaticObjectHolder::contextManager;

Context Context::getBaseContext()
{
	object cont = StaticObjectHolder::contextManager.attr("getDefaultContext")();
	return Context(extract<int>(cont.attr("ident")));
}

Context Context::getThreadContext()
{
	object cont = StaticObjectHolder::contextManager.attr("getThreadContext")();
	return Context(extract<int>(cont.attr("ident")));
}

std::string Context::operator () (std::string filename)
{
	object cont = StaticObjectHolder::contextManager.attr("getContext")(contextId);
	return extract<std::string>(cont.attr("str")(filename));
}
