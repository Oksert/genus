#ifndef __KERNEL_PERCEPTRON_H_
#define __KERNEL_PERCEPTRON_H_

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>

#include "Binary_Model.h"
#include <vector>
#include <ParsedContent.h>
#include <Measure.h>
#include <Logger.h>
#define dataset 3//1 -yeast, 2 - reuters, 3 - web

using namespace std;

/**
 * Kernel Perceptron
 */
class Kernel_Perceptron 
{
	/**
	 * �������� ������������� (>=0)
	 */
	float lambda;
	/**
	 * �������� �������� (0,1/lambda)
	 */
	float eta;
	/**
	 * margin �������� (>=0)
	 */
	float ro;
	/**
	 * �������� ���������� ��������
	 */
	int T;
	/**
	 * ������������ - ����������� - �����. ����.����.
	 */
	float *alpha;
	/**
	 * ��������
	 */
	float b;

	/**
	 * ���������� ��������� ���������� 
	 */
	int num_of_train_docs;

	/**
	 * �������� �������� ��������� �� ����� ����������
     * ����������� - num_of_train_docs
	 */
	float *similarity;
	/**
	 * ���������� �������� ����������
	 */
	int num_of_test_docs;

	/**
	 * ����������� - num_of_test_docs
	 */
	int *target_predict;
	float *f_predict;

	/**
	 * �������� ������
	 */
	Binary_Model *binary_model;
	/**
	 * ������� ���������� ������ ������
	 */
	int platt;

	float** cash_measure;
	int do_cash_measure;

public:

	Kernel_Perceptron(void);
	virtual ~Kernel_Perceptron();

	float get_lambda() {return lambda;}
	void put_lambda(float i) {lambda=i;}

	float get_eta() {return eta;}
	void put_eta(float i) {eta=i;}

	float get_ro() {return ro;}
	void put_ro(float i) {ro=i;}

	int get_T() {return T;}
	void put_T(int t) {T=t;}

	void put_platt(int t) {platt=t;}

	int get_num_of_train_docs() {return num_of_train_docs;}
	void put_num_of_train_docs(int n) {num_of_train_docs=n;}

	int get_num_of_test_docs() {return num_of_test_docs;}
	void put_num_of_test_docs(int n) {num_of_test_docs=n;}
	
	float* get_alpha() {return alpha;}
	void put_alpha(float *ptr) {alpha=ptr;}

	float get_b() {return b;}
	void put_b(float i) {b=i;}

	Binary_Model *get_binary_model() {return binary_model;}
	void put_binary_model(Binary_Model *bm) {binary_model = bm;}

	void put_similarity(float *ptr) {similarity=ptr;}

	int* get_target_predict() {return target_predict;}
	float* get_f_predict() {return f_predict;}

	float** get_cash_measure() {return cash_measure;}
	void put_cash_measure(float **ptr) {cash_measure=ptr;}
	int get_do_cash_measure() {return do_cash_measure;}
	void put_do_cash_measure(int i) {do_cash_measure=i;}

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
	 * ��������
	 */
	void train(CModule::Measure *measure);	
	/**
	 * ����������
	 * measure_new - ����������� ���� �������� 
	 * number_of_docs - ���������� ����������� ����������
	 */
	void incrementive_learning(CModule::Measure * measure_new, int number_of_docs);
	/**
	 * ������������ �������� ������� ������� ��� ����������� ���������� ������ ������ 
	 */
	void predict_platt(CModule::Measure *measure);

	/**
	 * �������������
	 */
	void predict();
};
#endif

