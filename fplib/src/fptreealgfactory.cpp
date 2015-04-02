#include "fptreealgfactory.h"
#include "fptreealg.h"
using namespace std;
using namespace fplib;

FPTreeAlgFactory::FPTreeAlgFactory()
{
}

FPTreeAlgFactory::~FPTreeAlgFactory()
{
}

void FPTreeAlgFactory::save(const FPTreeAlg * alg, std::ostream & os)
{
	alg->save(os);
}

FPNode * FPTreeAlgFactory::loadNode(std::istream & is)
{
	FPNode * node = new FPNode();
	long id;
	is.read((char*)&id, sizeof(long));
	_id2node.insert(map<long, FPNode*>::value_type(id, node));

	is.read((char*)&(node->item), sizeof(ItemId));
	is.read((char*)&(node->support), sizeof(int));
	size_t size;
	is.read((char*)&size, sizeof(size_t));
	
	for(size_t i = 0; i < size; i++)
	{
		FPNode * childNode = loadNode(is);
		childNode->parent = node;
		node->item2fpNode.insert(Item2FPNode::value_type(childNode->item, childNode));
	}
	return node;
}

PrefixNode * FPTreeAlgFactory::loadPrefixNode(std::istream & is)
{
	PrefixNode * node = new PrefixNode();
	is.read((char*)&(node->id), sizeof(int));
	is.read((char*)&(node->item), sizeof(ItemId));
	is.read((char*)&(node->support), sizeof(int));
	size_t size;
	is.read((char*)&size, sizeof(size_t));
	
	for(size_t i = 0; i < size; i++)
	{
		PrefixNode * childNode = loadPrefixNode(is);
		childNode->parent = node;
		node->item2prefixNode.insert(Item2PrefixNode::value_type(childNode->item, childNode));
	}
	return node;
}



FPTreeAlg * FPTreeAlgFactory::load(std::istream & is)
{
	FPTreeAlg * alg = new FPTreeAlg();
	is.read((char*)&(alg->_minSupport), sizeof(int));
	is.read((char*)&(alg->_totalCases), sizeof(int));
	is.read((char*)&(alg->_maxItemsetLen), sizeof(int));
	is.read((char*)&(alg->_itemsetsGrowthOnlyByTrainUp), sizeof(bool));
	is.read((char*)&(alg->_noSupportLimitForTree), sizeof(bool));
	is.read((char*)&(alg->_lastItemsetId), sizeof(int));

	// FP-tree
	_id2node.clear();
	alg->_fpTree = new FPTree(new HeaderTable());
	// root
	alg->_fpTree->root = loadNode(is);
	// header-table
	size_t size;
	is.read((char*)&size, sizeof(size_t));
	for(size_t i = 0; i < size; i++)
	{
		ItemId item;
		is.read((char*)&item, sizeof(ItemId));
		HeaderTableValue value;
		is.read((char*)&(value.support), sizeof(int));
		size_t size;
		is.read((char*)&size, sizeof(size_t));
		for(size_t j = 0; j < size; j++)
		{
			long id;
			is.read((char*)&id, sizeof(long));
			value.nodeLinks.push_back(_id2node.find(id)->second);
		}
		alg->_fpTree->headerTable->item2value.insert(map<ItemId, HeaderTableValue>::value_type(item, value));
	}
	// itemsets
	alg->_prefixTree = loadPrefixNode(is);

	//is.read((char*)&size, sizeof(size_t));
	//for(size_t i = 0; i < size; i++)
	//{
	//	
	//	SortedItems * sortedItems = new SortedItems();
	//	Itemset * itemset = new Itemset();
	//	is.read((char*)&(itemset->_support), sizeof(int));
	//	size_t size;
	//	is.read((char*)&size, sizeof(size_t));
	//	is.read((char*)&(itemset->_id), sizeof(int));
	//	for(size_t j = 0; j < size; j++)
	//	{
	//		ItemId item;
	//		is.read((char*)&item, sizeof(ItemId));
	//		sortedItems->push_back(item);
	//	}
	//	itemset->_sortedItems = sortedItems;
	//	alg->_itemsets.insert(itemset);
	//}
	return alg;
}
