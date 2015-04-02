#include "tfilereader.h"
#include "supportgreater.h"
#include <algorithm>

using namespace std;
using namespace fplib;

TFileReader::TFileReader(istream & is) 
: _is(is)
{
}

TFileReader::~TFileReader()
{
}


bool TFileReader::read(Items  & itemset, int & support)
{
	int c;
	while(true)
	{
		if((c = _is.peek()) == '\r')
			_is.ignore(1, c);

		if((c = _is.peek()) == '\n')
		{
			_is.ignore(1, c);
			support = 1;
			return true;
		}

		if(_is.eof())
			return false;

		int itemId;
		_is >> itemId;

		//if(_is.eof())
		//	return false;

		itemset.insert(itemId);

		_is.ignore(1, ' ');

	}
}

void TFileReader::moveFirst()
{
	_is.clear();
	_is.seekg(0, ios::beg);
}

