#pragma once
#include "headertable.h"
#include "iserializable.h"


namespace fplib
{

	struct Node;

	// FP-дерево
	struct FPTree : public ISerializable
	{
		HeaderTable * headerTable;
		FPNode * root;
	public:
		FPTree(HeaderTable * headerTable);
		virtual ~FPTree();
		virtual void save(std::ostream & os) const;
	};
}
