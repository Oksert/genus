#pragma once
#include "fptree.h"
#include "prefixnode.h"
#include "iserializable.h"
#include "sorteditems.h"


namespace fplib
{

	struct IDataReader;
	class FPTreeAlgFactory;

	/*!
	 * Алгоритм поиска частых шаблонов FP-tree
	 */
	class FPTreeAlg : public ISerializable
	{
		friend class FPTreeAlgFactory;
	private:
		int _minSupport;
		int _totalCases;
		int _maxItemsetLen;
		bool _itemsetsGrowthOnlyByTrainUp;
		bool _noSupportLimitForTree;
		FPTree * _fpTree;
		PrefixNode * _prefixTree;
		int _lastItemsetId;
		//Itemsets _itemsets;
		

		// запрет копирования
		FPTreeAlg(const FPTreeAlg & alg);
		FPTreeAlg & operator=(const FPTreeAlg & alg);


		// создание headerTable
		void firstPass(IDataReader * reader, int & totalCases, HeaderTable *headerTable);
		// создание FP-tree
		void secondPass(IDataReader * reader, FPTree * fpTree);
		// генерация частых наборов
		void mine(const FPTree * fpTree, const SortedItems & base);

		void free();
		/*void _train(IDataReader * reader);*/
		void freeItemsets();

		void add2prefixTree(PrefixNode * node, const SortedItems & items, SortedItems::const_iterator begin,
							   int support);

	public:
		/*!
		 * Конструктор
		 */
		FPTreeAlg();

		/*!
		 * Деструктор
		 */
		virtual ~FPTreeAlg();

		/*!
		 * Порог поддержки
		 *
		 * \returns
		 * Минимальное значение поддержки в виде числа транзакций
		 */
		int & minSupport();

		/*!
		 * Порог поддержки
		 *
		 * \returns
		 * Минимальное значение поддержки в виде числа транзакций
		 */
		int minSupport() const;

		/*!
		 * Максимальная длина набора
		 *
		 * \returns
		 * Максимальная длина набора
		 */
		int & maxItemsetLen();

		/*!
		 * Максимальная длина набора
		 *
		 * \returns
		 * Максимальная длина набора
		 */
		int maxItemsetLen() const;

		/*!
		 * Настройка "При дообучении множество наборов только расширяется"
		 *
		 * \returns
		 * Истина - в случае, если при дообучении множество наборов только расширяется.
		 *
		 * \remarks
		 * Фактически это запрет удаления наборов, которые стали редкими в результате дообучения.
		 */
		bool & itemsetsGrowthOnlyByTrainUp();

		/*!
		 * Настройка "При дообучении множество наборов только расширяется"
		 *
		 * \returns
		 * Истина - в случае, если при дообучении множество наборов только расширяется.
		 *
		 * \remarks
		 * Фактически это запрет удаления наборов, которые стали редкими в результате дообучения.
		 */
		bool itemsetsGrowthOnlyByTrainUp() const;

		/*!
		 * Настройка "Отсутствие порога по поддержке при построении частотного дерева"
		 *
		 * \returns
		 * Истина - в случае, если порог при поддержке частотного дерева отсутствует
		 *
		 * \remarks
		 * Отсутствие порога может быть полезно в тех случаях, когда модель требуется дообучать,
		 * и набор частых наборов, получаемых при каждом дообучении, должен быть актуальным.
		 * (Иначе наборы могут теряться в ситуации: при первом обучении (дообучении) набор был
		 * исключен из дерева из-за низкой частоты, при дообучении набор стал частым на суммарном
		 * множестве обучающих векторов, однако не является частым на "дообучающем" наборе.
		 */
		bool & noSupportLimitForTree();

		/*!
		 * Настройка "Отсутствие порога по поддержке при построении частотного дерева"
		 *
		 * \returns
		 * Истина - в случае, если порог при поддержке частотного дерева отсутствует
		 *
		 * \remarks
		 * Отсутствие порога может быть полезно в тех случаях, когда модель требуется дообучать,
		 * и набор частых наборов, получаемых при каждом дообучении, должен быть актуальным.
		 * (Иначе наборы могут теряться в ситуации: при первом обучении (дообучении) набор был
		 * исключен из дерева из-за низкой частоты, при дообучении набор стал частым на суммарном
		 * множестве обучающих векторов, однако не является частым на "дообучающем" наборе.
		 */
		bool noSupportLimitForTree() const;

		/*!
		 * Частые наборы.
		 *
		 * \returns
		 * Префиксное дерево частых наборов.
		 *
		 * \remarks
		 * При дообучении это фактически множество "текущих" частых наборов.
		 */
		const PrefixNode * prefixTree() const;

		/*!
		 * Число обработанных векторов.
		 *
		 * \returns
		 * Число обработанных векторов.
		 *
		 * \remarks
		 * Число обработанных векоторов за все обучения-дообучения.
		 */
		int totalCases() const;

		
		/*!
		 * Обучение модели
		 *
		 * \param reader Читатель базы данных
		 */
		void train(IDataReader * reader);

		/*!
		 * Дообучение модели
		 *
		 * \param reader Читатель базы данных
		 */
		void trainUp(IDataReader * reader);

		virtual void save(std::ostream & os) const;
	};
}
