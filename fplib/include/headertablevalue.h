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
	 * Элемент header-table
	 *
	 * \remarks
	 * На каждой итерации алгоритма объект класса
	 * HeaderTableValue является "источником данных" для следующей итерации
	 */
	struct HeaderTableValue : 
		public IDataReader, public ISerializable
	{
	private:
		NodeLinks::const_iterator _currIt;
	public:
		friend class SupportGreater;
		friend struct HeaderTable;

		//! Поддержка
		int support;
		
		//! node-links - списки вершин FP-дерева
		NodeLinks nodeLinks;

		/*!
		 * Конструктор.
		 *
		 * \param support Поддержка
		 */
		HeaderTableValue(int support = 0);

		//! Деструктор
		virtual ~HeaderTableValue();
		virtual bool read(Items  & itemset, int & support);
		virtual void moveFirst();
		virtual void save(std::ostream & os) const;
	};
}
