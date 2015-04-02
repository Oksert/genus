#include "fptreealg.h"
#include "idatareader.h"
#include "supportgreater.h"
#include <algorithm>
#include <limits>


//#include <map>
using namespace std;
using namespace fplib;

FPTreeAlg::FPTreeAlg() 
: _fpTree(0),
  _prefixTree(0),
_maxItemsetLen(numeric_limits<int>::max()),
_itemsetsGrowthOnlyByTrainUp(false),
_noSupportLimitForTree(false),
_lastItemsetId(0)
{
}



void FPTreeAlg::freeItemsets()
{
	delete _prefixTree;
	_prefixTree = 0;
}


void FPTreeAlg::free()
{
	_totalCases = 0;
	_lastItemsetId = 0;
	if(_fpTree != 0)
	{
		delete _fpTree;
		_fpTree = 0;
	}
	freeItemsets();
}

FPTreeAlg::~FPTreeAlg()
{
	free();
}

int & FPTreeAlg::minSupport()
{
	return _minSupport;
}

int FPTreeAlg::minSupport() const
{
	return _minSupport;
}

int & FPTreeAlg::maxItemsetLen()
{
	return _maxItemsetLen;
}

int FPTreeAlg::maxItemsetLen() const
{
	return _maxItemsetLen;
}

bool & FPTreeAlg::itemsetsGrowthOnlyByTrainUp()
{
	return _itemsetsGrowthOnlyByTrainUp;
}

bool FPTreeAlg::itemsetsGrowthOnlyByTrainUp() const
{
	return _itemsetsGrowthOnlyByTrainUp;
}

bool & FPTreeAlg::noSupportLimitForTree()
{
	return _noSupportLimitForTree;
}

bool FPTreeAlg::noSupportLimitForTree() const
{
	return _noSupportLimitForTree;
}

const PrefixNode * FPTreeAlg::prefixTree() const
{
	return _prefixTree;
}


void FPTreeAlg::firstPass(IDataReader * reader, int & totalCases, HeaderTable *headerTable)
{
	reader->moveFirst();
	
	// сбор статистики встречаемости
	while(true)
	{
		Items items;
		int support;
		if(!reader->read(items, support))
			break;
		totalCases++;
		for(Items::const_iterator itemIt = items.begin(); itemIt != items.end();itemIt++)
			headerTable->Add(*itemIt, support);

	}

	if(!_noSupportLimitForTree)
	{

		// уничтожение нечастых
		for(map<ItemId, HeaderTableValue>::iterator it = headerTable->item2value.begin(); it != headerTable->item2value.end();)
		{
			if(it->second.support < _minSupport)
				headerTable->item2value.erase(it++);
			else
				it++;
		}
	}

	// сортировка по убыванию частоты
	headerTable->Copy();
	headerTable->Sort();
}

void FPTreeAlg::secondPass(IDataReader * reader, FPTree * fpTree)
{
	reader->moveFirst();
	//FPTree * fpTree = new FPTree(headerTable);
	while(true)
	{
		Items items;
		int support;
		if(!reader->read(items, support))
			break;
		SortedItems sortedItems;
		for(Items::const_iterator itemIt = items.begin(); itemIt != items.end(); itemIt++)
			if(fpTree->headerTable->item2value.find(*itemIt) != fpTree->headerTable->item2value.end())
				sortedItems.push_back(*itemIt);
		sort(sortedItems.begin(), sortedItems.end(), SupportGreater(*fpTree->headerTable));
		FPNode * node = fpTree->root;
		for(vector<ItemId>::const_iterator it = sortedItems.begin(); it != sortedItems.end(); it++)
		{
			FPNode * newNode;
			Item2FPNode::iterator foundIt = node->item2fpNode.find(*it);
			if(foundIt == node->item2fpNode.end())
			{
				newNode = new FPNode(*it, node, support);
				node->item2fpNode.insert(Item2FPNode::value_type(*it, newNode));
				fpTree->headerTable->item2value.find(*it)->second.nodeLinks.push_back(newNode);
			}
			else
			{
				newNode = foundIt->second;
				newNode->support += support;
			}
			node = newNode;
		}


	}
	//return fpTree;


}


void FPTreeAlg::add2prefixTree(PrefixNode * node, const SortedItems & items, SortedItems::const_iterator begin,
							   int support)
{
	PrefixNode * child = node->getChild(*begin);
	if(child == 0)
	{
		child = new PrefixNode(*begin, node, ++_lastItemsetId);
		node->item2prefixNode.insert(Item2PrefixNode::value_type(*begin, child));
	}
	begin++;
	if(begin == items.end())
	{
		child->support = support;
		return;
	}
	add2prefixTree(child, items, begin, support);
}


void FPTreeAlg::mine(const FPTree * fpTree, const SortedItems & base)
{
	// ѕроход по всем частым Item начина€ с редких
	for(vector<ItemId>::const_reverse_iterator it = fpTree->headerTable->sortedItems.rbegin(); 
		it != fpTree->headerTable->sortedItems.rend(); it++)
	{ 
		/*const*/ HeaderTableValue & v = fpTree->headerTable->item2value.find(*it)->second;

		if(v.support < _minSupport)
			continue;

		// подсчет поддержки и отсеивание редких
		
		int totalCases = 0;
		FPTree * newTree = new FPTree(new HeaderTable());
		firstPass(&v, totalCases, newTree->headerTable);
		// создание conditional tree
		secondPass(&v, newTree);

		SortedItems newBase(base);
		newBase.push_back(*it);

		
		sort(newBase.begin(), newBase.end());


		if(v.support >= _minSupport)
		{
			add2prefixTree(_prefixTree, newBase, newBase.begin(), v.support);
			//Itemset * newItemset = new Itemset(newBase, v.support);
			//pair<Itemsets::iterator, bool> pair = newItemsets.insert(newItemset);
			//if(!pair.second)
			//{
			//	(*(pair.first))->_support += v.support;
			//	delete newItemset;
			//}
		}

		if(newTree->root->item2fpNode.size() > 0 && newBase.size() < _maxItemsetLen )
			mine(newTree, newBase);

		delete newTree;
	}
}



//void FPTreeAlg::_train(IDataReader * reader)
//{
//	firstPass(reader, _totalCases, _fpTree->headerTable);
//	secondPass(reader, _fpTree);
//	mine(_fpTree, &SortedItems());
//}

void FPTreeAlg::train(IDataReader * reader)
{
	free();
	_fpTree = new FPTree(new HeaderTable());
	_prefixTree = new PrefixNode();

	firstPass(reader, _totalCases, _fpTree->headerTable);
	secondPass(reader, _fpTree);
	mine(_fpTree, SortedItems());

	//// раздача id
	//for(Itemsets::iterator it = _itemsets.begin(); it != _itemsets.end(); it++)
	//	(*it)->_id = ++_lastItemsetId;
}

void FPTreeAlg::trainUp(IDataReader * reader)
{
	if(!_itemsetsGrowthOnlyByTrainUp)
	{
		freeItemsets();
		_prefixTree = new PrefixNode();
	}

	firstPass(reader, _totalCases, _fpTree->headerTable);
	secondPass(reader, _fpTree);

//	Itemsets newItemsets;
	mine(_fpTree, SortedItems());
	
	//for(Itemsets::const_iterator it = newItemsets.begin(); it != newItemsets.end(); it++)
	//{
	//	Itemsets::iterator foundIt = _itemsets.find(*it);
	//	if(foundIt == _itemsets.end())
	//	{
	//		(*it)->_id = ++_lastItemsetId;
	//	}
	//	else
	//	{
	//		(*it)->_id = (*foundIt)->_id;
	//		delete *foundIt;
	//		_itemsets.erase(foundIt);
	//	}
	//	_itemsets.insert(*it);
	//}

}


//FPTree * FPTreeAlg::_createdFilteredBySupportTree() const
//{
//	HeaderTable * newHeaderTable = new HeaderTable();
//	for(map<ItemId, HeaderTableValue>::const_iterator it = _fpTree->headerTable->begin();
//		it != _fpTree->headerTable->end(); it++)
//		if(it->second.support >= _minSupport)
//			newHeaderTable->Add(it->first, it->second.support
//
//	FPTree * newTree = new FPTree();
//}




void FPTreeAlg::save(ostream & os) const
{
	os.write((char*)&_minSupport, sizeof(int));
	os.write((char*)&_totalCases, sizeof(int));
	os.write((char*)&_maxItemsetLen, sizeof(int));
	os.write((char*)&_itemsetsGrowthOnlyByTrainUp, sizeof(bool));
	os.write((char*)&_noSupportLimitForTree, sizeof(bool));
	os.write((char*)&_lastItemsetId, sizeof(int));

	_fpTree->save(os);
	
	_prefixTree->save(os);

}
