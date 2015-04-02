#pragma once
#include "itemid.h"
#include "iserializable.h"
#include <map>

namespace fplib
{

	struct FPNode;
	typedef std::map<ItemId, FPNode*> Item2FPNode;

	/*!
	 * ������� FP-������
	 */
	struct FPNode : public ISerializable
	{
		//! item
		ItemId item;
		
		//! ������������ �������
		const FPNode * parent;
		
		//! ���������
		int support;
		
		//! ������������� ������ item -> FPNode
		Item2FPNode item2fpNode;

		/*!
		 * �����������
		 *
		 * \param item item
		 * \param parent ������������ �������
		 * \param support ���������
		 *
		 */
		FPNode(ItemId item = 0, const FPNode * parent = 0, int support = 0);
		virtual ~FPNode();
		virtual void save(std::ostream & os) const;
	};
}
