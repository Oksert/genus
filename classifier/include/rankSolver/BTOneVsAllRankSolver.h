#ifndef BTONEVSALLRANKSOLVER_H_
#define BTONEVSALLRANKSOLVER_H_

#include <cmath>

#include "Definitions.h"
#include "helper/VectorHelper.h" 
#include "rankSolver/AbstractRankSolver.h"

class BTOneVsAllRankSolver : public AbstractRankSolver
{
public:
	virtual NumberType* solveRanks(const DecompositionMatrix& D, NumberType* probs, int* docs_count);
protected:
	static const NumberType eps;
	static const NumberType dif;
	static const NumberType step;
	
	inline bool eq(NumberType a, NumberType b) const
	{
		return (fabs(a-b) < eps);
	}
	
	inline NumberType calculateComponent(NumberType r, NumberType s) const
	{
		return ( (1+s) - sqrt( (1+s)*(1+s) - 4*s*r) ) / (2 * s);
	}
	
	NumberType calculateFunctiom(int nr_class, NumberType* r, NumberType s) const;
	
	NumberType* findRoots(int nr_class, NumberType* r) const;
	
	int findFirstAppearance(const DecompositionMatrix& D, int nr_binary, int token) const;
};

#endif /*BTONEVSALLRANKSOLVER_H_*/
