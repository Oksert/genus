#pragma once

namespace DictionaryDB
{
	class Data_Element {
	public:
		Data_Element(int ident, int frequency);
		~Data_Element();
		int freq;
		int id;
		bool link;
	};
}
