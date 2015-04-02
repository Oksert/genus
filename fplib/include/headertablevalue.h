#pragma once
#include<list>
#include "fpnode.h"
#include "idatareader.h"
#include "iserializable.h"

namespace fplib
{

	class SupportGreater;

	typedef std::list<const FPNode*> NodeLinks;

	/*!
	 * ������� header-table
	 *
	 * \remarks
	 * �� ������ �������� ��������� ������ ������
	 * HeaderTableValue �������� "���������� ������" ��� ��������� ��������
	 */
	struct HeaderTableValue : 
		public IDataReader, public ISerializable
	{
	private:
		NodeLinks::const_iterator _currIt;
	public:
		friend class SupportGreater;
		friend struct HeaderTable;

		//! ���������
		int support;
		
		//! node-links - ������ ������ FP-������
		NodeLinks nodeLinks;

		/*!
		 * �����������.
		 *
		 * \param support ���������
		 */
		HeaderTableValue(int support = 0);

		//! ����������
		virtual ~HeaderTableValue();
		virtual bool read(Items  & itemset, int & support);
		virtual void moveFirst();
		virtual void save(std::ostream & os) const;
	};
}
