#include "headertable.h"
#include "supportgreater.h"
#include <algorithm>


	using namespace std;
	using namespace fplib;


	HeaderTable::HeaderTable()
	{
	}

	void HeaderTable::Add(ItemId itemId, int support)
	{
		item2value[itemId].support += support;
	}


	void HeaderTable::Copy()
	{
		for(map<ItemId, HeaderTableValue>::const_iterator it = item2value.begin(); 
			it != item2value.end(); it++)
			sortedItems.push_back(it->first);
	}

	void HeaderTable::Sort()
	{
		sort(sortedItems.begin(), sortedItems.end(), SupportGreater(*this));
	}


	void HeaderTable::Clear()
	{
		item2value.clear();
		sortedItems.clear();
	}

	void HeaderTable::save(ostream & os) const
	{
		size_t size = item2value.size();
		os.write((char*)&size, sizeof(size_t));
		for(vector<ItemId>::const_iterator itemIt = sortedItems.begin(); itemIt != sortedItems.end(); itemIt++)
		{
			os.write((char*)&(*itemIt), sizeof(ItemId));
			item2value.find(*itemIt)->second.save(os);
		}
	}
