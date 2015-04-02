#pragma once
#include "itemid.h"
#include "iserializable.h"
#include <map>

namespace fplib
{

	struct PrefixNode;
	typedef std::map<ItemId, PrefixNode*> Item2PrefixNode;

	/*!
	 * Вершина префиксного дерева
	 */
	struct PrefixNode : public ISerializable
	{
		//! item
		ItemId item;
		
		//! Родительская вершина
		const PrefixNode * parent;
		
		//! Поддержка
		int support;

		//! Идентификатор набора
		int id;
		
		//! Ассоциативный массив item -> PrefixNode
		Item2PrefixNode item2prefixNode;

		/*!
		 * Конструктор
		 *
		 * \param item item
		 * \param parent Родительская вершина
		 * \param id Идентификатор item
		 * \param support Поддержка
		 *
		 */
		PrefixNode(ItemId item = 0, const PrefixNode * parent = 0, int id = 0, int support = 0);

		/*!
		 * Дочерняя вершина по item id
		 * 
		 * \param itemId item id
		 * 
		 * \returns
		 * Дочерняя вершина, соответствующая itemId либо 0, если не найдена
		 */
		PrefixNode * getChild(ItemId itemId) const;

		virtual ~PrefixNode();
		virtual void save(std::ostream & os) const;
	};
}
