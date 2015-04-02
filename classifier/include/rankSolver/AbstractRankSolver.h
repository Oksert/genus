#ifndef ABSTRACT_RANK_SOLVER_H_
#define ABSTRACT_RANK_SOLVER_H_

#include "Definitions.h"
#include "decompositionMatrix/DecompositionMatrix.h"

class AbstractRankSolver
{
public:
	virtual NumberType* solveRanks(const DecompositionMatrix& D, NumberType*  probs, int* docs_count) = 0;
	
	//virtual bool isDecompositionSupported(int decomp_type) = 0;
protected:
	//TODO
	NumberType* makeRanksFromProbs(int n, NumberType* probs, int* docs_count)
	{
		NumberType* r = new NumberType[n];
		
		for (int c=0; c<n; ++c)
			r[c] = probs[c] * docs_count[c];
		
		return r;
	}
	
	void makeBTDModelParameters(int nr_binary,  NumberType*  probs, int* docs_count,
								NumberType*& r, NumberType*& r_, NumberType*& t)
	{
		int half = nr_binary / 2;
		for(int i = 0; i < half; ++i)
		{
			r[i]  = probs[i] * (1 - probs[i+half]);
			r_[i] = (1- probs[i]) * probs[i+half];
			t[i]  = (1- probs[i]) * (1 - probs[i+half]) + probs[i] * probs[i+half];
			
			r[i]  *= docs_count[i] + docs_count[i+half];
			r_[i] *= docs_count[i] + docs_count[i+half];
			t[i]  *= docs_count[i] + docs_count[i+half];

			r[i]  /= 2;
			r_[i] /= 2;
			t[i]  /= 2;
		}
	}
};

#endif /*ABSTRACT_RANK_SOLVER_H_*/
