#include "rankSolver/BTOneVsAllRankSolver.h"

const NumberType BTOneVsAllRankSolver::eps  = 0.000001;
const NumberType BTOneVsAllRankSolver::dif  = 0.001;
const NumberType BTOneVsAllRankSolver::step = 10;

NumberType* BTOneVsAllRankSolver::solveRanks(const DecompositionMatrix& D, NumberType* probs, int* docs_count)
{
	NumberType* t = findRoots(D.getClassCount(), probs);
	
	NumberType* r = new NumberType[D.getClassCount()];
	
	for (int c=0; c < D.getClassCount(); ++c)
		r[findFirstAppearance(D, c, ONLY_CLASS)] = t[c];

	delete [] t;
	
	return r;
}

NumberType BTOneVsAllRankSolver::calculateFunctiom(int nr_class, NumberType* r, NumberType s) const
{
	NumberType res = 0;
	
	if (eq(s,0))
	{
		for (int c = 0; c < nr_class; c++)
			res += r[c];
	}
	else
	{
		for (int c = 0; c < nr_class; c++)
			res += calculateComponent(r[c],s);
	}
	
	return res - 1;
}

NumberType* BTOneVsAllRankSolver::findRoots(int nr_class, NumberType* r) const
{
	NumberType left = 0, right = 0, mid;
	
	if ( eq( VectorHelper::componentSum(nr_class, r), 1) )
	{
		return VectorHelper::makeCopy(nr_class, r);
	}
	else
	{
		if (calculateFunctiom(nr_class, r, 0) > 0)
		{
			right = step;
			while (calculateFunctiom(nr_class, r, right) > 0)
				right += step;
		}
		else
		{
			left = -step;
			while (calculateFunctiom(nr_class, r, left) < 0)
				left -= step;
		}
		
		mid = (right+left)/2;
		
		NumberType fm;
		NumberType fr = calculateFunctiom(nr_class, r, right);
		
		while ( std::abs(right-left)>dif )
		{
			fm = calculateFunctiom(nr_class, r, mid  );
			
			if ( fr * fm > 0 )
			{
				right = mid;
				
				fr = fm;
			}
			else
				left  = mid;
			
			mid = (right+left)/2;
		}
	}
	
	NumberType* res = new NumberType[nr_class];
	
	for(int c=0; c<nr_class; ++c)
	{
		res[c] = calculateComponent(r[c], mid);
	}
	
	return res;
}

int BTOneVsAllRankSolver::findFirstAppearance(const DecompositionMatrix& D, int nr_binary, int token) const
{
	for(int i=0; i < D.getClassCount(); ++i)
		if (D[nr_binary][i] == token)
			return i;

	return -1;
}
