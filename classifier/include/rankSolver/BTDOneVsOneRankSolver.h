#ifndef BTDONEVSONERANKSOLVER_H_
#define BTDONEVSONERANKSOLVER_H_

#include "helper/VectorHelper.h"
#include "rankSolver/AbstractRankSolver.h"

class BTDOneVsOneRankSolver : public AbstractRankSolver
{
public:
	virtual NumberType* solveRanks(const DecompositionMatrix& D, NumberType*  probs, int* docs_count);
};

#endif /*BTDONEVSONERANKSOLVER_H_*/
