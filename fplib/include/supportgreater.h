#pragma once

#include "headertable.h"

namespace fplib
{
	/*!
	 * Отношение "меньше" для элементов заданной header-table
	 */
	class SupportGreater
	{
	private:
		const HeaderTable & _headerTable;
	public:
		/*!
		 * Конструктор.
		 *
		 * \param headerTable header-table
		 */
		SupportGreater(const HeaderTable & headerTable);
		bool operator()(ItemId id1, ItemId id2) const;
	};
}
