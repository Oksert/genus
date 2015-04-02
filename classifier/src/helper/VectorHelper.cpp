#include "helper/VectorHelper.h"

#include <ctime>
#include <cstdlib>
#include <cstring>

#include <boost/random.hpp>

using boost::mt19937;
using boost::variate_generator;
using boost::normal_distribution;

NumberType VectorHelper::componentSum(int n, NumberType* r)
{
	NumberType s = 0;

	for (int i=0; i < n; ++i)
		s += r[i];

	return s;
}

void VectorHelper::normalizeRanks(int n, NumberType* r)
{
	NumberType s = componentSum(n, r);

	for (int i=0; i < n; ++i)
		r[i] /= s;
}

NumberType* VectorHelper::makeCopy(int n, NumberType* r)
{
	NumberType* res = new NumberType[n];

	memcpy(res, r, n * sizeof(NumberType));

	return res;
}


float VectorHelper::GHD(int* v1, int* v2, int n)
{
	float r = 0;
	
	for (int i=0; i<n; ++i)
		r += abs( v1[i] - v2[i] ) / 2.0;
	
	return r;
}

int VectorHelper::rankingLoss(int n, NumberType* v1, NumberType* v2)
{
	int r = 0;
	
	for(int f = 0; f < n-1; f ++)
		for (int s = f+1; s < n; ++s)
		{
			if ( (v1[f] <= v1[s]) && (v2[f] > v2[s]) )
				++r;
			else if ( (v1[f] >= v1[s]) && (v2[f] < v2[s]) )
				++r;
			else if ( (v1[f] != v1[s]) && (v2[f] == v2[s]) )
				++r;
		}
	
	return r;
}

NumberType VectorHelper::placeRankingLoss(int n, NumberType* v1, NumberType* v2)
{
	NumberType r = 0;
	
	NumberType det = n - 1;
	
	for(int f = 0; f < n-1; f ++)
		for (int s = f+1; s < n; ++s)
		{
			if ( (v1[f] <= v1[s]) && (v2[f] > v2[s]) )
				r += (s - f) / det;
			else if ( (v1[f] >= v1[s]) && (v2[f] < v2[s]) )
				r += (s - f) / det;
			else if ( (v1[f] != v1[s]) && (v2[f] == v2[s]) )
				r += (s - f) / det;
		}
	
	return r;
}

void VectorHelper::addNoise(int n, NumberType* v, int level, NumberType zero)
{
	NumberType sd;
	
	static mt19937 rng(static_cast<unsigned> (time(0)));
	
	for (int i=0; i < n; ++i)
	{
		sd = v[i] * (NumberType(level)/100);
		
		normal_distribution<NumberType> norm_dist(0.0, sd);
		variate_generator<mt19937&, normal_distribution<NumberType> >  normal_sampler(rng, norm_dist);
		
		v[i] += normal_sampler();
		
		if (v[i] <= 0.0)
			v[i] = zero;
		else if (v[i] >= 1.0)
			v[i] = 1.0;
	}
}
