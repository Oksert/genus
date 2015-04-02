#pragma once
#include "headertablevalue.h"
#include "iserializable.h"

#include <vector>
#include <map>


namespace fplib
{

	/*!
	 * Таблица отсортированных по убыванию частоты списков вершин в FP-tree
	 * для заданных item'ов
	 */
	struct HeaderTable : public ISerializable
	{
		//! item -> частота и NodeLinks
		std::map<ItemId, HeaderTableValue> item2value;

		//! Отсортированные по частоте item'ы
		std::vector<ItemId> sortedItems;

		//! Конструктор
		HeaderTable();
		
		/*!
		 * Добавление нового item 
		 *
		 * \param itemId  Идентификатор item'а
		 * \param support Поддержка
		 */
		void Add(ItemId itemId, int support);
		
		//! Копирование item2value в sortedItems
		void Copy();
		
		//! Сортировка sortedItems
		void Sort();
		
		//! Очистка всего
		void Clear();

		virtual void save(std::ostream & os) const;
	};
}
