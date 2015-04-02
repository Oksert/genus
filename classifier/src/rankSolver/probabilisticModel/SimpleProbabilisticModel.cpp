#include "rankSolver/probabilisticModel/SimpleProbabilisticModel.h"

#include "helper/VectorHelper.h"

using std::pair;

std::vector<std::string> SimpleProbabilisticModel::getQualityParametrs()
{
	std::vector<std::string> names(2);
	
	names[0] = "RankingLoss";
	names[1] = "PlaceRankingLoss";
	
	return names;
}

void SimpleProbabilisticModel::init(int nr_class)
{
	this->~SimpleProbabilisticModel();
	
	this->nr_class = nr_class;
	
	rng     = new mt19937*[nr_class];
	sampler = new variate_generator<mt19937&, uniform_real<NumberType> >*[nr_class];
	
	for(int i=0; i<nr_class; i++)
	{
		rng[i]     = new mt19937( rand()%1000000 );
		sampler[i] = new variate_generator<mt19937&, uniform_real<NumberType> >(*(rng[i]), distr);
	}
}

int SimpleProbabilisticModel::generateProbabilies()
{
	NumberType* probs = new NumberType[nr_class];

	//Случайным образом выбрать вероятности принадлежности классов к текущему документу
	for (int c=0; c<nr_class; ++c)
		probs[c] = (*(sampler[c]))();
	
	int id = currentID++;
	(this->probs).insert( pair<int, NumberType*>(id, probs) );
	
	return id;
}

NumberType* SimpleProbabilisticModel::generateBinaryProbabilies(const DecompositionMatrix& M, int id)
{
	NumberType* probs = (this->probs)[id];
	 
	NumberType* r = new NumberType[M.getBinaryCount()];
	
	for(int b=0; b < M.getBinaryCount(); ++b)
	{
		NumberType only_mult = 1.0, rest_mult = 1.0;
		
		for (int c = 0; c < nr_class; ++c)
		{
			if (M[b][c] == ONLY_CLASS)
			{
				only_mult *= (1-probs[c]);
			}
			else if (M[b][c] == REST_CLASS)
			{
				rest_mult *= (1-probs[c]);
			}
		}
		
		r[b] = rest_mult * (1.0 - only_mult);
	}
	
	return r;
}

void SimpleProbabilisticModel::estimateQuality(NumberType* ranks, int id, NumberType*& result)
{
	NumberType* probs = (this->probs)[id];
	
	result[0] = VectorHelper::rankingLoss(nr_class, probs, ranks);
	result[1] = VectorHelper::placeRankingLoss(nr_class, probs, ranks);
}

void SimpleProbabilisticModel::releaseID(int id)
{
	delete [] ((this->probs)[id]);
	
	probs.erase(id);
}

SimpleProbabilisticModel::~SimpleProbabilisticModel()
{
	std::map<int, NumberType*>::iterator it;
	
	for(int i=0; i<nr_class; i++)
	{
		delete [] sampler[i];
		delete [] rng[i];
	}
	
	if (!sampler) delete sampler;
	if (!rng) delete rng;
	
	for ( it=probs.begin() ; it != probs.end(); it++ )
	{
		delete [] (*it).second;
	}
}
