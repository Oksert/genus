#ifndef __DOCUMENT_SET_H_
#define __DOCUMENT_SET_H_

#include "Classified_Document.h"

#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace std;

/** 
 * набор классифицированных документов
 */
class Document_Set
{
	/**
	 * количество документов
	 */
	int number_of_documents;
	/**
	 * массив классифицированных документов
	 */
	Classified_Document *documents;
public:
	Document_Set();
	virtual ~Document_Set();

	int get_number_of_documents() { return number_of_documents; }
	void put_number_of_documents(int num) { number_of_documents = num;}

	Classified_Document* get_documents() {return documents;}
	void put_documents(Classified_Document *ptr) { documents=ptr;}

	void read (char *fname);
	void save (char *fname);

	void show ();
};

#endif
