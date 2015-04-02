#pragma once
#include "fptreealg.h"
#include <iostream>
#include <map>

namespace fplib
{

	class FPTreeAlg;

	struct FPNode;

	/*!
	 * ������� �������� ���� FPTreeAlg.
	 *
	 * \remarks
	 * � ������� ������� ������ ���������� / �������� �������.
	 */
	class FPTreeAlgFactory
	{
	private:
		std::map<long, const FPNode *> _id2node;
		FPNode * loadNode(std::istream & is);
		PrefixNode * loadPrefixNode(std::istream & is);
	public:
		/*!
		 * �����������.
		 */
		FPTreeAlgFactory();
		/*!
		 * �������� ������
		 *
		 * \param is ������� �����
		 *
		 * \returns
		 * ������
		 */
		FPTreeAlg * load(std::istream & is);
		/*!
		 * ���������� ������
		 *
		 * \param alg ������
		 * \param os �������� �����
		 */
		void save(const FPTreeAlg * alg, std::ostream & os);
		/*!
		 * ����������.
		 */
		virtual ~FPTreeAlgFactory();
	};
}
