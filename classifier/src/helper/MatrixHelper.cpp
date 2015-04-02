#include "helper/MatrixHelper.h"

int MatrixHelper::getMaxNumberOfBinary(const vector<DecompositionMatrix*>& D)
{
	if (D.size() == 0)
		return -1;
	
	int max = D[0]->getBinaryCount();
	
	for(int i=1; i < D.size(); ++i)
		if (D[i]->getBinaryCount() > max)
			max = D[i]->getBinaryCount();
	
	return max;
}
