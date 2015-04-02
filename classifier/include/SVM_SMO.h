#ifndef __SVM_SMO_H_
#define __SVM_SMO_H_

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>

#include "Binary_Model.h"
#include <vector>
#include <ParsedContent.h>
#include <Measure.h>
#include "Logger.h"
#define dataset 3 //1 -yeast, 2 - reuters, 3 - web

using namespace std;

/**
 * Sequential Minimal Optimization
 * ����� ������� �������� (� ����������� ���������������� �����������)
 */
class SMO 
{
	/**
	 * ������ ������������� - ���������� ��������,
	 * ����������� ������� - ���������� ������� ��������
	 */
	float * alpha;
	/**
	 * ��� ������,
	 * ����������� - ���������� ������� ��������
	 */
	float * error_cache;
	/**
	 * �������� �������������
	 */
	float C;
	/**
	 * ��������
	 */
	float b;
	/**
	 * �������� ���������� �������� ����������� 
	 */
	float eps;
	/**
	 * ���������� ���������� � �������� �������-����-������
	 */
	float tolerance;
	int numChanged;
	int examineAll;

	/**
	 * ���������� ������� ��������
	 */	
	int number_of_SV;

	/**
	 * �������� ������
	 */
	Binary_Model *binary_model;
		
	/**
	 * ����� ������� ��� ��������� ����������,
	 * ����������� - ���������� ������� ��������
	 */
	int *target;

	/**
	 * �������� �������� ��������� �� ����� ����������
	 * ����������� - ���������� ������� ��������
	 */
	float *similarity;
	
	/**
	 * ���������� �������� ����������
	 */
	int num_of_test_docs;

	/**
	 * ������������� �����
	 * ����������� - ���������� �������� ����������
	 */
	int *target_predict;
	/**
	 * ������������� ������������ �������� ������� �������
	 * ����������� - ���������� �������� ����������
	 */
	float *f_predict;
	
	/**
	 * ���� - ��������� �� ����� ������
	 */
	int platt;

	float** cash_measure;
	int do_cash_measure;

public:

	SMO();
	virtual ~SMO();

	void put_platt(int t) {platt=t;}

	float get_C() {return C;}
	void put_C(float i) {C=i;}

	float* get_alph() {return alpha;}
	void put_alph(float *ptr) {alpha=ptr;}

	float get_b() {return b;}
	void put_b(float i) {b=i;}

	Binary_Model *get_binary_model() {return binary_model;}
	void put_binary_model(Binary_Model *bm) {binary_model = bm;}
		
	int get_number_of_SV() {return number_of_SV;}
	void put_number_of_SV(int i) {number_of_SV=i;}

	int get_number_of_test_docs() {return num_of_test_docs;}
	void put_number_of_test_docs(int n) {num_of_test_docs=n;}

	void put_similarity(float *ptr) {similarity=ptr;}

	int* get_target_predict() {return target_predict;}
	void put_target_predict(int *ptr) {target_predict=ptr;}

	float* get_f_predict() {return f_predict;}
	void put_f_predict(float *ptr) {f_predict=ptr;}

	float** get_cash_measure() {return cash_measure;}
	void put_cash_measure(float **ptr) {cash_measure=ptr;}
	int get_do_cash_measure() {return do_cash_measure;}
	void put_do_cash_measure(int i) {do_cash_measure=i;}

	/**
	 * ��������� ������� ������� f(x)=�����(alpha[i]*Y[i]*k(i,x))-b
	 */
	float learned_func_nonlinear(int k, CModule::Measure *measure);
	/**
	 * �������������� ������� ������� f(x)=�����(alpha[i]*k(i,x))-b
	 */
	float f();
	/**
	 * ���������� �������� ������� ������� ��� ��������� ����������
	 */
	float f_platt(int j, CModule::Measure *measure);

	/**
	 * ����� �������� �������������
	 */
	void write_alph();

	/**
	 * ���������� ����������� ���������� �������� i1 � i2
	 * ���������� 1, ���� ���������� ����������� i1 � i2 �������, 0 - �����
	 */
	int takeStep(int i1, int i2,CModule::Measure *measure);
	/**
	 * ���������� 1, ���� ����������� �����������; 0 - �����
	 */
	int examineExample(int i1,CModule::Measure *measure);
	int examineFirstChoice(int i1,float E1,CModule::Measure *measure);
	int examineNonBound(int i1,CModule::Measure *measure);
	int examineBound(int i1,CModule::Measure *measure);

	/**
	 * ��������
	 */
	void smo_train(CModule::Measure *measure);
	/**
	 * ������������ �������� ������� ������� ��� ����������� ���������� ������ ������
	 */
	void smo_predict_platt(CModule::Measure *measure);

	/**
	 * �������������
	 */
	void smo_predict();
	
};
#endif

