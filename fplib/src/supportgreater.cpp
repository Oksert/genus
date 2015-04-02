#include "supportgreater.h"

using namespace fplib;

SupportGreater::SupportGreater(const HeaderTable & headerTable)
: _headerTable(headerTable)
{
}

bool SupportGreater::operator()(ItemId id1, ItemId id2) const
{ 
	return _headerTable.item2value.find(id1)->second.support >
		   _headerTable.item2value.find(id2)->second.support;
}
