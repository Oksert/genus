#pragma once
#include<vector>
#include "itemid.h"

namespace fplib
{
	/*!
	 * Отсортированный набор
	 *
	 * \remarks
	 * Для сортировки используется std::sort() для вектора
	 */
	typedef std::vector<ItemId> SortedItems;
}
