#ifndef SIMPLETEAMRANKSOLVER_H_
#define SIMPLETEAMRANKSOLVER_H_

#include "helper/VectorHelper.h"
#include "rankSolver/AbstractRankSolver.h"

class SimpleTeamRankSolver : public AbstractRankSolver
{
public:
	virtual NumberType* solveRanks(const DecompositionMatrix& D, NumberType* probs, int* docs_count);
};

#endif /*SIMPLETEAMRANKSOLVER_H_*/
