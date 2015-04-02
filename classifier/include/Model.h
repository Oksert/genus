#ifndef __MODEL_H_
#define __MODEL_H_

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "Binary_Model.h"

using namespace std;
/**
 * тип multi-label декомпозиции
 */
enum {ONE_ONE, ONE_ALL, DENSE, SPARSE}; 

#define ONLY_CLASS  1
#define REST_CLASS -1
#define ZERO_CLASS  0

/**
 * multi-label модель
 */
class Model 
{
	/**
	 * количество тем
	 */
	int number_of_topics;
	/**
	 * количество бинарных подпроблем
	 */
	int number_of_binary_problems;
	/**
	 * тип декомпозиции на бинарные подпроблемы
	 */
	int multiclass_decomposition_type;
	/**
	 * матрица декомпозиции
	 */
	int **decomposition_matrix;
	/**
	 * количество опорных векторов
	 */
	int number_of_SV;
	/**
	 * массив опорных векторов
	 */
	Classified_Document *supp_vectors;
	/**
	 * набор бинарных подпроблем
	 */
	Binary_Model *set_of_binary_models;

public:
	Model();
	virtual ~Model();

	int get_number_of_topics() { return number_of_topics; }
	void put_number_of_topics(int num) { number_of_topics = num;}

	int get_number_of_binary_problems() { return number_of_binary_problems; }
	void put_number_of_binary_problems(int num) { number_of_binary_problems = num;}
	
	int get_multiclass_decomposition_type() { return multiclass_decomposition_type; }
	void put_multiclass_decomposition_type (int t) { multiclass_decomposition_type = t;}

	int** get_decomposition_matrix() {return decomposition_matrix;}
	void put_decomposition_matrix(int **ptr) {decomposition_matrix=ptr;}

	int get_number_of_SV() { return number_of_SV; }
	void put_number_of_SV(int num) { number_of_SV = num;}

	Classified_Document* get_supp_vectors() {return supp_vectors;}
	void put_supp_vectors(Classified_Document *sv) {supp_vectors = sv;}

	Binary_Model* get_binary_models() {return set_of_binary_models;}
	void put_binary_models(Binary_Model *bm) {set_of_binary_models = bm;}	

	/**
	 * удаление опорных векторов, 
	 * которым соответствуют нулевые коэффициенты
	 * во всех бинарных моделях
	 */
	void delete_zero();

	void read_model(char *f_model);
	void save_model(char *f_model);

	/**
	 * создать матрицу декомпозиции multi-label проблемы на бинарные подпроблемы
	 */
	void create_decomposition_matrix();

	void show();
};

#endif
