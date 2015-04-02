#ifndef __ML_CLASSIFICATION_H_
#define __ML_CLASSIFICATION_H_

#include <fstream>
#include <iostream>
#include <new>
#include <string>
#include <algorithm>

//#include <conio.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <stdarg.h>
#include <time.h>
#include <search.h>

#include "SVM_SMO.h"
#include "Kernel_Perceptron.h"

#include "Document.h"
#include "Classified_Document.h"
#include "Model.h"
#include "Decomposition.h"
#include "Logger.h"
using namespace std;

/**
 * ����� �������������
 */
enum {SVM, Kern_Perc};
/**
 * �������
 */
enum {TRAIN, PREDICT};

struct topics
{
	/**
	 * ���������� ����������� ���
	 */
	int number;
	/**
	 * ������ ������� ����������� ���  
	 */
	int *theames;
};

/**
 * MLClassification algorithm - �������� ����������� �������������  
 */
class MLClassification
{
	/**
	 * ��� ������������: 0 - (1vs1); 1 - (1 vs all)
	 */
	int decomposition_type;
	/**
	 * ����� �������� �������������: 0-SVM; 1- Kern_Perc
	 */
	int method;
	/**
	 * �������� ������������� ��� ���������� �������� �������������
	 */
	float reg_param;
	/**
	 * ������ �� �����-���������: 0 ���� 1
	 */
	int do_cross_validation;
	/**
	 * ��������� �����-���������
	 */
	float C_start,C_finish, step;
	/**
	 * ������ �� ������ ������������ ������� ������� ������: 0 ���� 1
	 */
	int do_Platt_probabilities;
	/**
	 * ������ �� ��������� ��� � ������� "��������" � ������ "������-������-�������"
	 * 0 - ��� ���������, 1 - � ����������  
	 */
	int cutting;
	/**
	 * ����� ��� ���������
	 */
	double threshold;
	/**
	 * ������������ ��������� ������� (1) ��� 
	 * ����������� ����� (0)
	 */
	int threshold_method;
	/**
	 * ��������� �� ��������� ��� ���������
	 * ����������� ���
	 */
	int heuristics;
	/**
	 * 0 - �� ������ ������������
	 * 1 - �� ������ ���� �������� 
	 */
	int threshold_function;
	/**
	 * ������������ ��������� ������� 
	 */
	float *ALS;
	/**
	 * ���� ��� ��������� ������� 
	 */
	int flag;
	
	/**
	 * ���������� ��������� ����������
	 */
	int num_of_train_docs;
	/**
	 * ���������� ���������� ��� �����-���������
	 */
	int num_of_CV_docs;
	/**
	 * ���������� �������� ���������� 
	 */
	int num_of_test_docs; 
	
	/**
	 * multi-label ������ 
	 */
	Model *model;

	/**
	 * ���������� ������� �������� � �������� �������  
	 */
	int *num_of_SV_in_b_models;

	/** 
	 * �������� ����� ����������  
	 */
	int **target_CV;
	int **target_test;

	/**
	 * ���������� ������ ��������� 
	 */
	float **MLC_ranking_f;
	int **MLC_result;
	float **MLC_ranking;
	int **MLC_ranks;
	/**
	 * ������������ �������� ���������
	 */
	float *ranking;
	
	/**
	 * ������ ������
	 */
	float Hamming_Loss;
	float One_Error;
	float coverage;
	float average_precision;
	float ranking_loss;

	/**
	 * ��� ������ ���������� ���������
	 */
	/**
	 * ������ ������������� ����������� ��������������, ���������� ��� ��������� �� h
	 */
	double * ALSN;
	/**
	 * ������ ���������� ������������� ����������� ��������� ��������������
	 */
	double * DA;
	/**
	 * ������ ��������� ��� ���������� ������������� ����������� ��������� ��������������
	 */
	double *GRAD;
	/**
	 * ����� ������� DA
	 */
	double NDA;

public:
	
	MLClassification();
	virtual ~MLClassification();

	/**
	 * �������� multi-label ��������������
	 * number_of_topics - ����� ���������� ��� � ������ �������������
	 * tops - ������ ��������, ���������� ���������� � ����������� ����� ��������� ����������
	 * measure - ���� �������� ��� ��������� ����������
	 */
	void train (int number_of_topics, topics *tops, CModule::Measure *measure);
	/**
	 * ���������� multi-label ��������������, ���������� ����
	 * number_of_topics - ����� ���������� ��� � ������ �������������
	 * number_of_IL_docs - ���������� ����������� ���������� (���� ����� ���� - �� ���������� ���������� ����)
	 * tops - ������ ��������, ���������� ���������� � ����������� ����� ����������
	 * measure - ����������� ���� �������� ��� ���� ����������
	 */
	void incrementive_learning (int number_of_topics, int number_of_IL_docs, topics *tops, CModule::Measure *measure);
	/**
	 * ������������ � multi-label �������������
	 * similarity - ������ �������� �������� ������ ��������� �� ����� ���������� �����������
	 * RETURN:
	 *	tops - ���������, � ������� ������������ ���������� � ����������� ����� 
	 *	(���������� ����������� ��� � ���� ����������� ����, ������������� � ������� �������������,
	 *	������� � ����� �����������)
	 *	ranks - ������ ������������ ���� ��� �� �������� �� ������������� ��� ������� ���������
     *	weights - ������ ����� ���, ������������� �� �������� ������������� 
	 */
	void predict(float *similarity, topics *tops, int *ranks, float *weights);

	/**
	 * �������� ����
	 * topic_number - ����� ��������� ���� (��������� �� ����)
	 * number_of_topics - ����� ���������� ��� � ������ �������������
	 * number_of_docs - ���������� ��������� ����������
	 * tops - ������ ��������, ���������� ���������� � ����������� ����� ��� ��������� ����������
	 * measure - ���� �������� ��� ���� ��������� ����������
	 */
	void delete_topic(int topic_number, int number_of_topics, 
				int number_of_docs, topics *tops, CModule::Measure *measure);

	//������������ �� �������� ������ ��� �����������
	void decomposition_inc(int num_of_tops, topics *tops, CModule::Measure *measure);

	/**
	 * �������� k-�� �������� ������ �� ����� bmodel0000k
	 */
	void read_b_model(Binary_Model *b_model, int k);
	/**
	 * ���������� k-�� �������� ������ � ���� bmodel0000k 
	 */
	void save_b_model(Binary_Model *b_model, int k);
	/**
	 * ������������ �� �������� �����������   
	 */
	void decomposition(int num_of_tops, topics *tops, CModule::Measure *measure);
	/**
	 * ������� ���������� ������� ��������
	 */
	void calc_number_of_SV(int num_of_tops, topics *tops, CModule::Measure *measure);
	/**
	 * ������������ ���� ��������
	 */
	float** cash_measure;
	/**
	 * ������ �������� ����������� �������������
	 */
	float HammingLoss (int *rtv, int *predict, int num_of_tops);
	/**
	 * �������� ������ �������� ������������
	 * "������ � �������� ����������� ����"
	 */
	int OneError(int *rtv, int* R, int num_of_tops);
	/**
	 * "��������" ���� ����������� ���
	 */
	int Coverage(int *rtv, int* R, int num_of_tops);
	/**
	 * ���������� �������� ������������ 
	 */
	float AveragePrecision(int *rtv, int* R, int num_of_tops);
	/**
	 * ������ ������������ ����������� � ������������� ���
	 */
	float RankingLoss(int *rtv, int* R, int num_of_tops);

	//--------------------------------------------------------------
	/**
	 * ����� ������ ��� ���������� �������������� ������ �� ����������� �������� ������������� 
	 * ������� ��������
	 */
	void sigmoid_train(int l, float *dec_values, int *labels, 
							float& A, float& B);
	/**
	 * ������� ������������
	 */
	float sigmoid_predict(float decision_value, float A, float B);
	//--------------------------------------------------------------
	/**
	 * ������������ ������������ ������� ��� ������ "������-������-�������"
	 */
	void predict_probs(int test_j, float* rank, float *A, float *B, Model *model);
	//--------------------------------------------------------------
	void put_decomposition_type(int i) {decomposition_type=i;}
	void put_method(int m) {method=m;}
	void put_do_cross_validation(int i) {do_cross_validation=i;}
	void put_do_Platt_probabilities(int i) {do_Platt_probabilities=i;}
	void put_cutting(int i) {cutting=i;}
	void put_threshold(double t) {threshold=t;}
	void put_model(Model* m) {model=m;}

	void put_num_of_train_docs(int n) {num_of_train_docs=n;}
	void put_num_of_CV_docs(int n) {num_of_CV_docs=n;}
	void put_num_of_test_docs(int n) {num_of_test_docs=n;}

	void put_target_CV(int **ptr) {target_CV=ptr;}
	void put_target_test(int **ptr) {target_test=ptr;}
	//-------------------------------------------------------------------

	void train_threshold (int number_of_topics, int number_of_SV, topics *tops, CModule::Measure *measure);
	int threshold_error(double t, int num_of_tops, topics tops, float * ranking);
	void least_squares(double** X, double * T, int dim);
	void save_vector (float *v, int dim, char *f_mod);
	void read_vector (float *v, int dim, char *f_mod);
	void rename_b_model(int k, int l);
	void remove_b_model(int k);
	void load_models();	

	void GRADIENT(double** X, double* T, int dim);
	double findMinDihotomy(double a,double b,double sigma, double* T, int dim, double **X);
	double TF (double h, double* T, int dim, double **X);
};

#endif
