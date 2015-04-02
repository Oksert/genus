#pragma once
#include "items.h"

namespace fplib
{

	/*!
	 * ��������� "��������" ������
	 *
	 * \remarks
	 *
	 * � �������� "���������" ������ ��������� "����������" ������ TFileReader � HederTableValue
	 * 
	 * \see
	 * TFileReader | HederTableValue
	 */
	struct IDataReader
	{
		/*!
		 * ������� � ������ ������ ��
		 */
		virtual void moveFirst() = 0;

		/*!
		 * ������� ������� ���������� (� ���� ������ ��������������� item'��)
		 *
		 * \param itemset ��������� ����� item'��
		 * \param support ��������� ���������
		 *
		 * \returns
		 * ����, ���� ��������� ����� �� (� ���� ������ ���������� �� ����� ���� �������)
		 */
		virtual bool read(Items  & itemset, int & support) = 0;
		virtual ~IDataReader() {}
	};
}
