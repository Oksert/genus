#pragma once
#include "itemid.h"
#include "iserializable.h"
#include <map>

namespace fplib
{

	struct FPNode;
	typedef std::map<ItemId, FPNode*> Item2FPNode;

	/*!
	 * Вершина FP-дерева
	 */
	struct FPNode : public ISerializable
	{
		//! item
		ItemId item;
		
		//! Родительская вершина
		const FPNode * parent;
		
		//! Поддержка
		int support;
		
		//! Ассоциативный массив item -> FPNode
		Item2FPNode item2fpNode;

		/*!
		 * Конструктор
		 *
		 * \param item item
		 * \param parent Родительская вершина
		 * \param support Поддержка
		 *
		 */
		FPNode(ItemId item = 0, const FPNode * parent = 0, int support = 0);
		virtual ~FPNode();
		virtual void save(std::ostream & os) const;
	};
}
