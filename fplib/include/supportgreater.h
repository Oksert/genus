#pragma once

#include "headertable.h"

namespace fplib
{
	/*!
	 * ��������� "������" ��� ��������� �������� header-table
	 */
	class SupportGreater
	{
	private:
		const HeaderTable & _headerTable;
	public:
		/*!
		 * �����������.
		 *
		 * \param headerTable header-table
		 */
		SupportGreater(const HeaderTable & headerTable);
		bool operator()(ItemId id1, ItemId id2) const;
	};
}
