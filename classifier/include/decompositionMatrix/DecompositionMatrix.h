#ifndef DECOMPOSITIONMATRIX_H_
#define DECOMPOSITIONMATRIX_H_

#include <iostream>

typedef int binLable;

#define ONLY_CLASS  1
#define REST_CLASS -1
#define ZERO_CLASS  0

class DecompositionMatrixGenerator;

class DecompositionMatrix
{
	friend class DecompositionMatrixGenerator;
	friend std::ostream& operator << (std::ostream& os, const DecompositionMatrix& d);
protected:
	int nr_class;
	int nr_binary;
	
	binLable** m;
	
	static const float  BIN_SEP_FACTOR;
public:
	static DecompositionMatrix makeMirror(const DecompositionMatrix& d);
	static DecompositionMatrix makeFrom2Darray(int* m, int nr_binary, int nr_class);
	
	DecompositionMatrix(int nr_binary, int nr_class);
	
	DecompositionMatrix(const DecompositionMatrix& d);
	DecompositionMatrix(binLable** m, int nr_binary, int nr_class);
	
	DecompositionMatrix& operator = (const DecompositionMatrix& d);
	
	binLable* operator [] (int b) const {return m[b];};
	
	operator binLable**() const {return m;};
	
	int getClassCount() const  {return nr_class; };
	int getBinaryCount() const {return nr_binary;};
	
	int isCorrect() const;
	
	//ghd - generalized Hamming distance
	float ghdScore() const;
	float ghdScore(float binSepFactor) const;
	
	virtual ~DecompositionMatrix();
private:
	DecompositionMatrix() {};
};

#endif /*DECOMPOSITIONMATRIX_H_*/
