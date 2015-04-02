#include "headertablevalue.h"



using namespace std;
using namespace fplib;


HeaderTableValue::HeaderTableValue(int support) : support(support)
{
}

HeaderTableValue::~HeaderTableValue()
{
}

bool HeaderTableValue::read(Items  & itemset, int & support)
{
	if(_currIt == nodeLinks.end())
		return false;

	const FPNode * node = (*_currIt)->parent;
	support = (*_currIt)->support;
	while(node->parent != 0)
	{
		itemset.insert(node->item);
		node = node->parent;
	}
	_currIt++;
	return true;
}

void HeaderTableValue::moveFirst()
{
	_currIt = nodeLinks.begin();
}


void HeaderTableValue::save(ostream & os) const
{
	os.write((char*)&support, sizeof(int));
	size_t size =nodeLinks.size();
	os.write((char*)&size, sizeof(size_t));
	for(NodeLinks::const_iterator nodeIt = nodeLinks.begin(); nodeIt != nodeLinks.end(); nodeIt++)
	{
		long id = (long)*nodeIt;
		os.write((char*)&id, sizeof(long));
	}
}

