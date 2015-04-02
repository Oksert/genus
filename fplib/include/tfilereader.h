#pragma once
#include <iostream>
#include "idatareader.h"

namespace fplib
{
	/*!
	 * "Читатель" базы транзакций, заданной в файле
	 *
	 * \remarks
	 * Формат файла:
	 * Идентификаторы item'ов, разделенные пробелами.
	 * Граница транзакции - перевод строки.
	 */
	class TFileReader :
		public IDataReader
	{
	private:
		std::istream & _is;
	public:
		/*!
		 * Конструктор.
		 *
		 * \param is Входной поток
		 */
		TFileReader(std::istream & is);
		~TFileReader();
		virtual bool read(Items  & itemset, int & support);
		virtual void moveFirst();
	};
}
