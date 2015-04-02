#include "fpnode.h"

using namespace std;
using namespace fplib;


FPNode::FPNode(ItemId item, const FPNode * parent, int support)
: item(item), parent(parent), support(support)
{
}

FPNode::~FPNode()
{
	for(Item2FPNode::iterator it = item2fpNode.begin(); it != item2fpNode.end(); it++)
		delete it->second;
}


void FPNode::save(ostream & os) const
{
	long id = (long)this;
	os.write((char*)&id, sizeof(long));
	os.write((char*)&item, sizeof(ItemId));
	os.write((char*)&support, sizeof(int));
	size_t size = item2fpNode.size();
	os.write((char*)&size, sizeof(size_t));
	for(Item2FPNode::const_iterator it = item2fpNode.begin(); it!= item2fpNode.end(); it++)
		it->second->save(os);
}

