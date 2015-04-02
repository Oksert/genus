#pragma once
#include <iostream>
#include "idatareader.h"

namespace fplib
{
	/*!
	 * "��������" ���� ����������, �������� � �����
	 *
	 * \remarks
	 * ������ �����:
	 * �������������� item'��, ����������� ���������.
	 * ������� ���������� - ������� ������.
	 */
	class TFileReader :
		public IDataReader
	{
	private:
		std::istream & _is;
	public:
		/*!
		 * �����������.
		 *
		 * \param is ������� �����
		 */
		TFileReader(std::istream & is);
		~TFileReader();
		virtual bool read(Items  & itemset, int & support);
		virtual void moveFirst();
	};
}
