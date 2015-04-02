#ifndef MATRIXHELPER_H_
#define MATRIXHELPER_H_

#include <vector>

#include "Definitions.h"
#include "decompositionMatrix/DecompositionMatrix.h"

using std::vector;

class MatrixHelper
{
public:
	static int getMaxNumberOfBinary(const vector<DecompositionMatrix*>& D);
};

#endif /*MATRIXHELPER_H_*/
