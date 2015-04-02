#ifndef DECOMPOSITION_MATRIX_GENERATOR_H_
#define DECOMPOSITION_MATRIX_GENERATOR_H_

#include "DecompositionMatrix.h"

class DecompositionMatrixGenerator
{
public:
	static DecompositionMatrix generateOneVsAllMatrix(int nr_class);
	static DecompositionMatrix generateOneVsOneMatrix(int nr_class);
	
	static DecompositionMatrix generateDenseMatrix(int nr_class);
	static DecompositionMatrix generateDenseMatrix(int nr_binary, int nr_class);
	
	static DecompositionMatrix generateSparseMatrix(int nr_class,  float zeroProb);
	static DecompositionMatrix generateSparseMatrix(int nr_binary, int nr_class, float zeroProb);
	
protected:
	static int dense[6];
	static int sparse[3];
	
	static void generateDenseMatrix (DecompositionMatrix& d);
	static void generateSparseMatrix(DecompositionMatrix& d, float zeroProb);

	static const float ZERO_PROB;
	static const int   ECOC_CYCLES;
};

#endif /*DECOMPOSITION_MATRIX_GENERATOR_H_*/
