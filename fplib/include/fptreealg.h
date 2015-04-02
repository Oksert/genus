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
	 * �������� ������ ������ �������� FP-tree
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
		

		// ������ �����������
		FPTreeAlg(const FPTreeAlg & alg);
		FPTreeAlg & operator=(const FPTreeAlg & alg);


		// �������� headerTable
		void firstPass(IDataReader * reader, int & totalCases, HeaderTable *headerTable);
		// �������� FP-tree
		void secondPass(IDataReader * reader, FPTree * fpTree);
		// ��������� ������ �������
		void mine(const FPTree * fpTree, const SortedItems & base);

		void free();
		/*void _train(IDataReader * reader);*/
		void freeItemsets();

		void add2prefixTree(PrefixNode * node, const SortedItems & items, SortedItems::const_iterator begin,
							   int support);

	public:
		/*!
		 * �����������
		 */
		FPTreeAlg();

		/*!
		 * ����������
		 */
		virtual ~FPTreeAlg();

		/*!
		 * ����� ���������
		 *
		 * \returns
		 * ����������� �������� ��������� � ���� ����� ����������
		 */
		int & minSupport();

		/*!
		 * ����� ���������
		 *
		 * \returns
		 * ����������� �������� ��������� � ���� ����� ����������
		 */
		int minSupport() const;

		/*!
		 * ������������ ����� ������
		 *
		 * \returns
		 * ������������ ����� ������
		 */
		int & maxItemsetLen();

		/*!
		 * ������������ ����� ������
		 *
		 * \returns
		 * ������������ ����� ������
		 */
		int maxItemsetLen() const;

		/*!
		 * ��������� "��� ���������� ��������� ������� ������ �����������"
		 *
		 * \returns
		 * ������ - � ������, ���� ��� ���������� ��������� ������� ������ �����������.
		 *
		 * \remarks
		 * ���������� ��� ������ �������� �������, ������� ����� ������� � ���������� ����������.
		 */
		bool & itemsetsGrowthOnlyByTrainUp();

		/*!
		 * ��������� "��� ���������� ��������� ������� ������ �����������"
		 *
		 * \returns
		 * ������ - � ������, ���� ��� ���������� ��������� ������� ������ �����������.
		 *
		 * \remarks
		 * ���������� ��� ������ �������� �������, ������� ����� ������� � ���������� ����������.
		 */
		bool itemsetsGrowthOnlyByTrainUp() const;

		/*!
		 * ��������� "���������� ������ �� ��������� ��� ���������� ���������� ������"
		 *
		 * \returns
		 * ������ - � ������, ���� ����� ��� ��������� ���������� ������ �����������
		 *
		 * \remarks
		 * ���������� ������ ����� ���� ������� � ��� �������, ����� ������ ��������� ���������,
		 * � ����� ������ �������, ���������� ��� ������ ����������, ������ ���� ����������.
		 * (����� ������ ����� �������� � ��������: ��� ������ �������� (����������) ����� ���
		 * �������� �� ������ ��-�� ������ �������, ��� ���������� ����� ���� ������ �� ���������
		 * ��������� ��������� ��������, ������ �� �������� ������ �� "�����������" ������.
		 */
		bool & noSupportLimitForTree();

		/*!
		 * ��������� "���������� ������ �� ��������� ��� ���������� ���������� ������"
		 *
		 * \returns
		 * ������ - � ������, ���� ����� ��� ��������� ���������� ������ �����������
		 *
		 * \remarks
		 * ���������� ������ ����� ���� ������� � ��� �������, ����� ������ ��������� ���������,
		 * � ����� ������ �������, ���������� ��� ������ ����������, ������ ���� ����������.
		 * (����� ������ ����� �������� � ��������: ��� ������ �������� (����������) ����� ���
		 * �������� �� ������ ��-�� ������ �������, ��� ���������� ����� ���� ������ �� ���������
		 * ��������� ��������� ��������, ������ �� �������� ������ �� "�����������" ������.
		 */
		bool noSupportLimitForTree() const;

		/*!
		 * ������ ������.
		 *
		 * \returns
		 * ���������� ������ ������ �������.
		 *
		 * \remarks
		 * ��� ���������� ��� ���������� ��������� "�������" ������ �������.
		 */
		const PrefixNode * prefixTree() const;

		/*!
		 * ����� ������������ ��������.
		 *
		 * \returns
		 * ����� ������������ ��������.
		 *
		 * \remarks
		 * ����� ������������ ��������� �� ��� ��������-����������.
		 */
		int totalCases() const;

		
		/*!
		 * �������� ������
		 *
		 * \param reader �������� ���� ������
		 */
		void train(IDataReader * reader);

		/*!
		 * ���������� ������
		 *
		 * \param reader �������� ���� ������
		 */
		void trainUp(IDataReader * reader);

		virtual void save(std::ostream & os) const;
	};
}
