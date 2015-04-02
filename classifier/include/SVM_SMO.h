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
 * Метод опорных векторов (с применением последовательных оптимизаций)
 */
class SMO 
{
	/**
	 * массив коэффициентов - множителей Лагранжа,
	 * размерность массива - количество опорных векторов
	 */
	float * alpha;
	/**
	 * кеш ошибок,
	 * размерность - количество опорных векторов
	 */
	float * error_cache;
	/**
	 * параметр регуляризации
	 */
	float C;
	/**
	 * смещение
	 */
	float b;
	/**
	 * параметр завершения процесса оптимизации 
	 */
	float eps;
	/**
	 * допустимое отклонение в условиях Картуша-Куна-Такера
	 */
	float tolerance;
	int numChanged;
	int examineAll;

	/**
	 * количество опорных векторов
	 */	
	int number_of_SV;

	/**
	 * бинарная модель
	 */
	Binary_Model *binary_model;
		
	/**
	 * метки классов для обучающих документов,
	 * размерность - количество опорных векторов
	 */
	int *target;

	/**
	 * схожесть текущего документа со всеми обучающими
	 * размерность - количество опорных векторов
	 */
	float *similarity;
	
	/**
	 * количество тестовых документов
	 */
	int num_of_test_docs;

	/**
	 * предсказанные метки
	 * размерность - количество тестовых документов
	 */
	int *target_predict;
	/**
	 * предсказанные вещественные значения целевой функции
	 * размерность - количество тестовых документов
	 */
	float *f_predict;
	
	/**
	 * флаг - применять ли метод Платта
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
	 * обучаемая целевая функция f(x)=сумма(alpha[i]*Y[i]*k(i,x))-b
	 */
	float learned_func_nonlinear(int k, CModule::Measure *measure);
	/**
	 * результирующая целевая функция f(x)=сумма(alpha[i]*k(i,x))-b
	 */
	float f();
	/**
	 * вычисление значений целевой функции для обучающих документов
	 */
	float f_platt(int j, CModule::Measure *measure);

	/**
	 * вывод значений коэффициентов
	 */
	void write_alph();

	/**
	 * совместная оптимизация множителей Лагранжа i1 и i2
	 * возвращает 1, если совместная оптимизация i1 и i2 успешна, 0 - иначе
	 */
	int takeStep(int i1, int i2,CModule::Measure *measure);
	/**
	 * возвращает 1, если оптимизация выполнилась; 0 - иначе
	 */
	int examineExample(int i1,CModule::Measure *measure);
	int examineFirstChoice(int i1,float E1,CModule::Measure *measure);
	int examineNonBound(int i1,CModule::Measure *measure);
	int examineBound(int i1,CModule::Measure *measure);

	/**
	 * обучение
	 */
	void smo_train(CModule::Measure *measure);
	/**
	 * предсказание значений целевой функции для дальнейшего применения метода Платта
	 */
	void smo_predict_platt(CModule::Measure *measure);

	/**
	 * классификация
	 */
	void smo_predict();
	
};
#endif

