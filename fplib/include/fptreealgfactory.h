#pragma once
#include "fptreealg.h"
#include <iostream>
#include <map>

namespace fplib
{

	class FPTreeAlg;

	struct FPNode;

	/*!
	 * Фабрика объектов типа FPTreeAlg.
	 *
	 * \remarks
	 * В функции фабрики входит сохранение / загрузка моделей.
	 */
	class FPTreeAlgFactory
	{
	private:
		std::map<long, const FPNode *> _id2node;
		FPNode * loadNode(std::istream & is);
		PrefixNode * loadPrefixNode(std::istream & is);
	public:
		/*!
		 * Конструктор.
		 */
		FPTreeAlgFactory();
		/*!
		 * Загрузка модели
		 *
		 * \param is Входной поток
		 *
		 * \returns
		 * Модель
		 */
		FPTreeAlg * load(std::istream & is);
		/*!
		 * Сохранение модели
		 *
		 * \param alg Модель
		 * \param os Выходной поток
		 */
		void save(const FPTreeAlg * alg, std::ostream & os);
		/*!
		 * Деструктор.
		 */
		virtual ~FPTreeAlgFactory();
	};
}
