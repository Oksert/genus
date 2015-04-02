#pragma once
#include <iostream>

namespace fplib
{

	/*!
	 * ����������� ��������
	 */
	struct ISerializable
	{
		virtual void save(std::ostream & os) const = 0;
	};
}
