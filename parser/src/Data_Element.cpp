#include "ParserStartup.h"
#include "DictionaryIncludes.h"

using namespace DictionaryDB;

Data_Element::Data_Element(int ident, int frequency) {
	id = ident;
	freq = frequency;
	link = false;
}

Data_Element::~Data_Element() {

}
