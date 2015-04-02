#pragma once
#include "headertablevalue.h"
#include "iserializable.h"

#include <vector>
#include <map>


namespace fplib
{

	/*!
	 * ������� ��������������� �� �������� ������� ������� ������ � FP-tree
	 * ��� �������� item'��
	 */
	struct HeaderTable : public ISerializable
	{
		//! item -> ������� � NodeLinks
		std::map<ItemId, HeaderTableValue> item2value;

		//! ��������������� �� ������� item'�
		std::vector<ItemId> sortedItems;

		//! �����������
		HeaderTable();
		
		/*!
		 * ���������� ������ item 
		 *
		 * \param itemId  ������������� item'�
		 * \param support ���������
		 */
		void Add(ItemId itemId, int support);
		
		//! ����������� item2value � sortedItems
		void Copy();
		
		//! ���������� sortedItems
		void Sort();
		
		//! ������� �����
		void Clear();

		virtual void save(std::ostream & os) const;
	};
}
