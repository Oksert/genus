#ifndef __DOCUMENT_H_
#define __DOCUMENT_H_

#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace std;

/**
 * неклассифицированный документ - последовательность лексем
 */
class Document 
{
protected:
	/**
	 * количество лексем
	 */
	int length;
	/**
	 * массив номеров лексем
	 */
	int *lexems;
public:
	Document();
	virtual ~Document();

	int get_length() { return length; }
	void put_length(int len) { length = len;}

	int* get_lexems() {return lexems;}
	void put_lexems(int *ptr) {lexems=ptr;}

	virtual void read(char *fname);
	virtual void save(char *fname);

	void show();
};

#endif

