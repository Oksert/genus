#include "prefixnode.h"

using namespace std;
using namespace fplib;


PrefixNode::PrefixNode(ItemId item, const PrefixNode * parent, int id, int support)
: item(item), parent(parent), id(id), support(support)
{
}

PrefixNode * PrefixNode::getChild(ItemId itemId) const
{
	Item2PrefixNode::const_iterator foundIt = item2prefixNode.find(itemId);
	return foundIt == item2prefixNode.end() ? 0 : foundIt->second;
}


PrefixNode::~PrefixNode()
{
	for(Item2PrefixNode::iterator it = item2prefixNode.begin(); it != item2prefixNode.end(); it++)
		delete it->second;
}


void PrefixNode::save(ostream & os) const
{
	//long id = (long)this;
	os.write((char*)&id, sizeof(int));
	os.write((char*)&item, sizeof(ItemId));
	os.write((char*)&support, sizeof(int));
	size_t size = item2prefixNode.size();
	os.write((char*)&size, sizeof(size_t));
	for(Item2PrefixNode::const_iterator it = item2prefixNode.begin(); it!= item2prefixNode.end(); it++)
		it->second->save(os);
}

