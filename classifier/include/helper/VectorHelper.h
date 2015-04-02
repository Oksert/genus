#ifndef VECTORHELPER_H_
#define VECTORHELPER_H_

#include "Definitions.h"

class VectorHelper
{
public:
	static NumberType  componentSum(int n, NumberType* r);
	
	static void        normalizeRanks(int n, NumberType* r);
	
	static NumberType* makeCopy(int n, NumberType* r);
	
	static float       GHD(int* v1, int* v2, int n);
	
	static int         rankingLoss(int n, NumberType* v1, NumberType* v2);
	
	static NumberType  placeRankingLoss(int n, NumberType* v1, NumberType* v2);
	
	static void        addNoise(int n, NumberType* v, int level, NumberType zero = 0.0001);
};

#endif /*VECTORHELPER_H_*/
