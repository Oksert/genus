#ifndef __CLASSIFIED_DOCUMENT_H_
#define __CLASSIFIED_DOCUMENT_H_

#include "Document.h"

#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace std;

/**
 * ������������������ �������� - ������������������ ������
 * � ���������� ������
 */
class Classified_Document : public Document
{
	/**
	 * ���������� ����������� ���
	 */
	int number_of_topics;
	/**
	 * ������ ������� ����������� ���
	 */
	int *topics;
public:
	Classified_Document();
	virtual ~Classified_Document();

	int get_number_of_topics() { return number_of_topics; }
	void put_number_of_topics(int num) { number_of_topics = num;}

	int* get_topics() {return topics;}
	void put_topics(int *ptr) {topics=ptr;}

	void cread (char *fname);
	void csave (char *fname);

	void cshow ();
};

#endif
