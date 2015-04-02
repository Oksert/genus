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
 * Декомпозиция - создаёт набор непросчитанных бинарных моделей
 * на основе набора классифицированных multi-label документов
 * Два основные варианта декомпозиции - "каждый-против-всех" и "каждый-против-каждого"
 */
class Decomposition 
{
	/**
	 * набор классифицированных multi-label документов для декомпозиции
	 */
	Document_Set *learning_docs;
	/** 
	 * количество тем
	 */
	int number_of_topics;
	/**
	 * количество бинарных проблем
	 */
	int number_of_binary_problems;
	/**
	 * количество опорных векторов
	 */
	int number_of_SV;
	/** 
	 * тип декомпозиции на бинарные проблемы
	 */
	int multiclass_decomposition_type;
	/**
	 * Матрица декомпозиции задаёт тип декомпозиции multi-label задачи на бинарные подпроблемы.
	 * Количество столбцов матрицы равно числу различных тем (классов), 
	 * количество строк равно количесву бинарных задач.
	 * Элемент матрицы равен +1, если документы соответствующего класса рассматриваются
	 * в данной бинарной подпроблеме как положительные;
	 *  -1, если как отрицательные, 0, если игнорируются.
	 */
	int **decomposition_matrix;
	/**
	 * набор бинарных подпроблем
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
	 * декомпозиция на бинарные подпроблемы
	 */
	void decompose();

	/**
	 * удаление опорных векторов, 
	 * которым соответствуют нулевые коэффициенты
	 * во всех бинарных моделях
	 */
	void delete_zero();
	
	/**
	 * сохраниение
	 */
	void save_decomposition();
	/**
	 * считывание
	 */
	void read_decomposition();

	void show();
};

#endif
