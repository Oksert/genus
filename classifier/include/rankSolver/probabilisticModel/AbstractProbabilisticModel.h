#ifndef ABSTRACTPROBABILISTICMODEL_H_
#define ABSTRACTPROBABILISTICMODEL_H_

#include <vector>
#include <string>

using std::vector;
using std::string;

#include "rankSolver/AbstractRankSolver.h"

class AbstractProbabilisticModel
{
public:
	virtual void init(int nr_class) = 0;
	
	virtual vector<string> getQualityParametrs() = 0;
	
	virtual int generateProbabilies() = 0;
	
	virtual NumberType* generateBinaryProbabilies(const DecompositionMatrix& M, int id) = 0;
	
	virtual void estimateQuality(NumberType* ranks, int id, NumberType*& result) = 0;
	
	virtual void releaseID(int id) = 0;
	
	//virtual ~AbstractProbabilisticModel();
};

#endif /*ABSTRACTPROBABILISTICMODEL_H_*/
