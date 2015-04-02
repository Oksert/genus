#ifndef SIMPLEPROBABILISTICMODEL_H_
#define SIMPLEPROBABILISTICMODEL_H_

#include <map>

#include <boost/random.hpp>

#include "rankSolver/probabilisticModel/AbstractProbabilisticModel.h"

using std::map;

using boost::mt19937;
using boost::variate_generator;
using boost::uniform_real;

class SimpleProbabilisticModel : public AbstractProbabilisticModel
{
protected:
	map<int, NumberType*> probs;
	
	int currentID;
	
	int nr_class;
	
	mt19937** rng;
	uniform_real<NumberType> distr;
	variate_generator<mt19937&, uniform_real<NumberType> >** sampler;

public:
	SimpleProbabilisticModel() : currentID(0), distr(0.0, 1.0), nr_class(0), rng(0), sampler(0) {};
	
	virtual void init(int nr_class);
	
	virtual vector<string> getQualityParametrs();
	
	virtual int generateProbabilies();
	
	virtual NumberType* generateBinaryProbabilies(const DecompositionMatrix& M, int id);
	
	virtual void estimateQuality(NumberType* ranks, int id, NumberType*& result);
	
	virtual void releaseID(int id);
	
	virtual ~SimpleProbabilisticModel();
};

#endif /*SIMPLEPROBABILISTICMODEL_H_*/
