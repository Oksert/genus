#include "fptree.h"

using namespace std;
using namespace fplib;

FPTree::FPTree(HeaderTable * headerTable)
: headerTable(headerTable)
{
	root = new FPNode();
}

FPTree::~FPTree()
{
	delete root;
	delete headerTable;
}


void FPTree::save(ostream & os) const
{
	root->save(os);
	headerTable->save(os);
}
