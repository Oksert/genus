#ifndef __BINARY_MODEL_H_
#define __BINARY_MODEL_H_

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>

#include "Document_Set.h"

using namespace std;

//enum {ONE_ONE, ONE_ALL}; // ��� ������������ //

/**
 * �������� ������
 */
class Binary_Model 
{
	/**
	 * 1 - �������������� ������ , 2 - ������������
	 */
	int state; 
	/**
	 * ���������� ������� �������� � ������ ��������
	 */
	int number_of_SV;
	/**
	 * ������ �������� ����� (0 ���� 1)
	 */
	int *labels;
	/**
	 * ����� � ����� ������ ������� ��������
	 */
	int *numbers; 
	
	/**
	 * ������������ ������� ��������
	 */
	float *coefficients;
	/**
	 * ��������
	 */
	float b;

	/**
	 * ������������ ��� ������ ������
	 */
	float A, B;

public:
	
	Binary_Model();
	virtual ~Binary_Model();

	int get_state() { return state; }
	void put_state(int st) { state = st;}

	int get_number_of_SV() { return number_of_SV; }
	void put_number_of_SV(int num) { number_of_SV = num;}

	int* get_labels() {return labels;}
	void put_labels(int *ptr) {labels=ptr;};

	int* get_numbers() {return numbers;}
	void put_numbers(int *ptr) {numbers=ptr;};

	float get_b() {return b;}
	void put_b(float i) {b=i;};

	float get_A() {return A;}
	void put_A(float i) {A=i;};

	float get_B() {return B;}
	void put_B(float i) {B=i;};

	float* get_coefficients() {return coefficients;}
	void put_coefficients(float *ptr) {coefficients=ptr;};

	/**
	 * ��������� ������ ��� ���������� ����� ���������� � �������� ��������
	 */
	void memory_allocate1(); 
	/**
	 * �������� �������� �������� �� ����� 
	 */
	void read(char *f_model);
	/**
	 * ���������� �������� �������� � ����
	 */
	void save(char *f_model);

	/** 
	 * �������� ������� ��������, ������� �������������
	 * ������� ������������
	 */
	void delete_zero();

	/**
	 * ���������� ������ �������� ������
	 */
	void show();
};

#endif
