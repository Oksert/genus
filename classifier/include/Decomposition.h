#ifndef __DECOMPOSITION_H_
#define __DECOMPOSITION_H_

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "Document_Set.h"
#include "Binary_Model.h"

using namespace std;

/**
 * ������������ - ������ ����� �������������� �������� �������
 * �� ������ ������ ������������������ multi-label ����������
 * ��� �������� �������� ������������ - "������-������-����" � "������-������-�������"
 */
class Decomposition 
{
	/**
	 * ����� ������������������ multi-label ���������� ��� ������������
	 */
	Document_Set *learning_docs;
	/** 
	 * ���������� ���
	 */
	int number_of_topics;
	/**
	 * ���������� �������� �������
	 */
	int number_of_binary_problems;
	/**
	 * ���������� ������� ��������
	 */
	int number_of_SV;
	/** 
	 * ��� ������������ �� �������� ��������
	 */
	int multiclass_decomposition_type;
	/**
	 * ������� ������������ ����� ��� ������������ multi-label ������ �� �������� �����������.
	 * ���������� �������� ������� ����� ����� ��������� ��� (�������), 
	 * ���������� ����� ����� ��������� �������� �����.
	 * ������� ������� ����� +1, ���� ��������� ���������������� ������ ���������������
	 * � ������ �������� ����������� ��� �������������;
	 *  -1, ���� ��� �������������, 0, ���� ������������.
	 */
	int **decomposition_matrix;
	/**
	 * ����� �������� ����������
	 */
	Binary_Model *binary_models;

public:
	Decomposition();
	virtual ~Decomposition();

	int get_number_of_topics() { return number_of_topics; }
	void put_number_of_topics(int num) { number_of_topics = num;}

	int get_number_of_binary_problems() { return number_of_binary_problems; }
	void put_number_of_binary_problems(int num) { number_of_binary_problems = num;}

	int get_multiclass_decomposition_type() { return multiclass_decomposition_type; }
	void put_multiclass_decomposition_type (int t) { multiclass_decomposition_type = t;}
	
	int get_number_of_SV() { return number_of_SV; }
	void put_number_of_SV(int num) { number_of_SV = num;}

	Document_Set* get_learning_docs() {return learning_docs;}
	void put_learning_docs(Document_Set *sv) {learning_docs = sv;}

	Binary_Model* get_binary_models() {return binary_models;}
	void put_binary_models(Binary_Model *bm) {binary_models = bm;}

	int** get_decomposition_matrix() {return decomposition_matrix;}
	void put_decomposition_matrix(int **ptr) {decomposition_matrix=ptr;}

	/**
	 * ������������ �� �������� �����������
	 */
	void decompose();

	/**
	 * �������� ������� ��������, 
	 * ������� ������������� ������� ������������
	 * �� ���� �������� �������
	 */
	void delete_zero();
	
	/**
	 * �����������
	 */
	void save_decomposition();
	/**
	 * ����������
	 */
	void read_decomposition();

	void show();
};

#endif
