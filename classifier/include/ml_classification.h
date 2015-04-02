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
 * метод классификации
 */
enum {SVM, Kern_Perc};
/**
 * процесс
 */
enum {TRAIN, PREDICT};

struct topics
{
	/**
	 * количество релевантных тем
	 */
	int number;
	/**
	 * массив номеров релевантных тем  
	 */
	int *theames;
};

/**
 * MLClassification algorithm - Алгоритм многотемной классификации  
 */
class MLClassification
{
	/**
	 * тип декомпозиции: 0 - (1vs1); 1 - (1 vs all)
	 */
	int decomposition_type;
	/**
	 * метод бинарной классфиикации: 0-SVM; 1- Kern_Perc
	 */
	int method;
	/**
	 * параметр регуляризации для алгоритмов бинарной классификации
	 */
	float reg_param;
	/**
	 * делать ли кросс-валидацию: 0 либо 1
	 */
	int do_cross_validation;
	/**
	 * параметры кросс-валидации
	 */
	float C_start,C_finish, step;
	/**
	 * делать ли оценку вероятностей классов методом Платта: 0 либо 1
	 */
	int do_Platt_probabilities;
	/**
	 * делать ли отсечение тем с низкими "голосами" в случае "каждый-против-каждого"
	 * 0 - без отсечения, 1 - с отсечением  
	 */
	int cutting;
	/**
	 * порог для отсечения
	 */
	double threshold;
	/**
	 * использовать пороговую функцию (1) или 
	 * константный порог (0)
	 */
	int threshold_method;
	/**
	 * применять ли эвристики при отсечении
	 * релевантных тем
	 */
	int heuristics;
	/**
	 * 0 - на основе ранжирования
	 * 1 - на основе меры схожести 
	 */
	int threshold_function;
	/**
	 * коэффициенты пороговой функции 
	 */
	float *ALS;
	/**
	 * флаг для пороговой функции 
	 */
	int flag;
	
	/**
	 * количество обучающих документов
	 */
	int num_of_train_docs;
	/**
	 * количество документов для кросс-валидации
	 */
	int num_of_CV_docs;
	/**
	 * количество тестовых документов 
	 */
	int num_of_test_docs; 
	
	/**
	 * multi-label модель 
	 */
	Model *model;

	/**
	 * количество опорных векторов в бинарных моделях  
	 */
	int *num_of_SV_in_b_models;

	/** 
	 * бинарные метки документов  
	 */
	int **target_CV;
	int **target_test;

	/**
	 * результаты работы алгоритма 
	 */
	float **MLC_ranking_f;
	int **MLC_result;
	float **MLC_ranking;
	int **MLC_ranks;
	/**
	 * ранжирование текущего документа
	 */
	float *ranking;
	
	/**
	 * оценка ошибок
	 */
	float Hamming_Loss;
	float One_Error;
	float coverage;
	float average_precision;
	float ranking_loss;

	/**
	 * для метода наименьших квадратов
	 */
	/**
	 * вектор коэффициентов разделяющей гиперплоскости, постоянный для дихотомии по h
	 */
	double * ALSN;
	/**
	 * вектор приращений коэффициентов разделяющей пороговой гиперплоскости
	 */
	double * DA;
	/**
	 * вектор градиента для приращений коэффициентов разделяющей пороговой гиперплоскости
	 */
	double *GRAD;
	/**
	 * норма вектора DA
	 */
	double NDA;

public:
	
	MLClassification();
	virtual ~MLClassification();

	/**
	 * обучение multi-label классификатора
	 * number_of_topics - общее количество тем в задаче классификации
	 * tops - массив структур, содержащих информацию о релевантных темах обучающих документов
	 * measure - мера сходства для обучающих документов
	 */
	void train (int number_of_topics, topics *tops, CModule::Measure *measure);
	/**
	 * дообучение multi-label классификатора, добавление темы
	 * number_of_topics - новое количество тем в задаче классификации
	 * number_of_IL_docs - количестов дообучающих документов (если равно нулю - то происходит добавление темы)
	 * tops - массив структур, содержащих информацию о релевантных темах документов
	 * measure - пополненная мера сходства для всех документов
	 */
	void incrementive_learning (int number_of_topics, int number_of_IL_docs, topics *tops, CModule::Measure *measure);
	/**
	 * ранжирование и multi-label классификация
	 * similarity - массив значений схожести нового документа со всеми обучающими документами
	 * RETURN:
	 *	tops - структура, в которую записывается информация о релевантных темах 
	 *	(количество релевантных тем и сами релевантные темы, упорядоченные в порядке релевантности,
	 *	начиная с самой релевантной)
	 *	ranks - массив ранжирования всех тем по убыванию их релевантности для данного документа
     *	weights - массив весов тем, упорядоченных по убыванию релевантности 
	 */
	void predict(float *similarity, topics *tops, int *ranks, float *weights);

	/**
	 * удаление темы
	 * topic_number - номер удаляемой темы (считается от нуля)
	 * number_of_topics - новое количество тем в задаче классификации
	 * number_of_docs - количестов обучающих документов
	 * tops - массив структур, содержащих информацию о релевантных темах для обучающих документов
	 * measure - мера сходства для всех обучающих документов
	 */
	void delete_topic(int topic_number, int number_of_topics, 
				int number_of_docs, topics *tops, CModule::Measure *measure);

	//декомпозиция на бинарные модели при доообучении
	void decomposition_inc(int num_of_tops, topics *tops, CModule::Measure *measure);

	/**
	 * загрузка k-ой бинарной модели из файла bmodel0000k
	 */
	void read_b_model(Binary_Model *b_model, int k);
	/**
	 * сохранение k-ой бинарной модели в файл bmodel0000k 
	 */
	void save_b_model(Binary_Model *b_model, int k);
	/**
	 * декомпозиция на бинарные подпроблемы   
	 */
	void decomposition(int num_of_tops, topics *tops, CModule::Measure *measure);
	/**
	 * подсчёт количества опорных векторов
	 */
	void calc_number_of_SV(int num_of_tops, topics *tops, CModule::Measure *measure);
	/**
	 * кешированная мера сходства
	 */
	float** cash_measure;
	/**
	 * оценка качества многотемной классификации
	 */
	float HammingLoss (int *rtv, int *predict, int num_of_tops);
	/**
	 * критерии оценки качества ранжирования
	 * "ошибка в наиболее релевантной теме"
	 */
	int OneError(int *rtv, int* R, int num_of_tops);
	/**
	 * "покрытие" всех релевантных тем
	 */
	int Coverage(int *rtv, int* R, int num_of_tops);
	/**
	 * усреднённая точность ранжирования 
	 */
	float AveragePrecision(int *rtv, int* R, int num_of_tops);
	/**
	 * ошибка упорядочения релевантных и нерелевантных тем
	 */
	float RankingLoss(int *rtv, int* R, int num_of_tops);

	//--------------------------------------------------------------
	/**
	 * метод Платта для вычисления вероятностного вывода по результатам бинарной классификации 
	 * процесс обучения
	 */
	void sigmoid_train(int l, float *dec_values, int *labels, 
							float& A, float& B);
	/**
	 * процесс предсказания
	 */
	float sigmoid_predict(float decision_value, float A, float B);
	//--------------------------------------------------------------
	/**
	 * предсказание вероятностей классов для случая "каждый-против-каждого"
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
