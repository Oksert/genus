#pragma once
#include <iostream>

namespace fplib
{

	/*!
	 * Сохраняемая сущность
	 */
	struct ISerializable
	{
		virtual void save(std::ostream & os) const = 0;
	};
}
