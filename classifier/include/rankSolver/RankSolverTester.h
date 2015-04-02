#ifndef RANKSOLVERTESTER_H_
#define RANKSOLVERTESTER_H_

#include <map>
#include <string>
#include <vector>

#include "Definitions.h"
#include "rankSolver/AbstractRankSolver.h"
#include "rankSolver/probabilisticModel/AbstractProbabilisticModel.h"

using std::map;
using std::string;
using std::vector;

class RankSolverTester
{
public:
	RankSolverTester(int t, int n, AbstractProbabilisticModel& m) : times(t), noise(n), model(m) {};
	
	map<string, vector<NumberType> > testRankSolvers
		(const vector<AbstractRankSolver*>& rs, const vector<DecompositionMatrix*>& D, const vector<int*>& docs_count) const;
	
	map<string, vector<NumberType> > testRankSolvers
		(const vector<AbstractRankSolver*>& rs, const vector<DecompositionMatrix*>& D, int Docs_count) const;
	
protected:
	int noise;//%
	int times;
	AbstractProbabilisticModel& model;
	
private:
	RankSolverTester() : model( *((AbstractProbabilisticModel*)(0)) ) {};
};

#endif /*RANKSOLVERTESTER_H_*/
