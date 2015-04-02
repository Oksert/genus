#pragma once
#include "itemid.h"
#include "iserializable.h"
#include <map>

namespace fplib
{

	struct PrefixNode;
	typedef std::map<ItemId, PrefixNode*> Item2PrefixNode;

	/*!
	 * ������� ����������� ������
	 */
	struct PrefixNode : public ISerializable
	{
		//! item
		ItemId item;
		
		//! ������������ �������
		const PrefixNode * parent;
		
		//! ���������
		int support;

		//! ������������� ������
		int id;
		
		//! ������������� ������ item -> PrefixNode
		Item2PrefixNode item2prefixNode;

		/*!
		 * �����������
		 *
		 * \param item item
		 * \param parent ������������ �������
		 * \param id ������������� item
		 * \param support ���������
		 *
		 */
		PrefixNode(ItemId item = 0, const PrefixNode * parent = 0, int id = 0, int support = 0);

		/*!
		 * �������� ������� �� item id
		 * 
		 * \param itemId item id
		 * 
		 * \returns
		 * �������� �������, ��������������� itemId ���� 0, ���� �� �������
		 */
		PrefixNode * getChild(ItemId itemId) const;

		virtual ~PrefixNode();
		virtual void save(std::ostream & os) const;
	};
}
