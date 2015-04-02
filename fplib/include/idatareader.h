#pragma once
#include "items.h"

namespace fplib
{

	/*!
	 * Интерфейс "читателя" данных
	 *
	 * \remarks
	 *
	 * В качестве "читателей" данных возникают "конкретные" классы TFileReader и HederTableValue
	 * 
	 * \see
	 * TFileReader | HederTableValue
	 */
	struct IDataReader
	{
		/*!
		 * Перейти к первой записи БД
		 */
		virtual void moveFirst() = 0;

		/*!
		 * Считать текущую транзакцию (в виде набора идентификаторов item'ов)
		 *
		 * \param itemset Считанный набор item'ов
		 * \param support Считанная поддержка
		 *
		 * \returns
		 * Ложь, если достигнут конец БД (в этом случае транзакция не может быть считана)
		 */
		virtual bool read(Items  & itemset, int & support) = 0;
		virtual ~IDataReader() {}
	};
}
