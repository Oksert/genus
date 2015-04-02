#include "rankSolver/SimpleTeamRankSolver.h"

NumberType* SimpleTeamRankSolver::solveRanks(const DecompositionMatrix& D, NumberType*  probs, int* docs_count)
{
	int* comp_count     = new int[D.getClassCount()];
	
	NumberType* ranking = new NumberType[D.getClassCount()];
	
	for (int c=0; c < D.getClassCount(); ++c)
	{
		comp_count[c] = 0;
		ranking[c]    = 0;
	}
	
	for (int b=0; b < D.getBinaryCount() / 2; b++)
	{//probs[b] - вероятность 1 в b бинарной проблемме
		int oc_count = 0, rc_count = 0;
		
		for (int c = 0; c < D.getClassCount(); ++c)
		{
			if ( D[b][c] == ONLY_CLASS )
			{
				++oc_count;
			}
			else if ( D[b][c] == REST_CLASS )
				++rc_count;
		}
		
		for (int c = 0; c < D.getClassCount(); ++c)
		{
			if ( D[b][c] == ONLY_CLASS )
			{
				ranking[c] += probs[b] / oc_count;
				comp_count[c]++;
			}
			else if ( D[b][c] == REST_CLASS )
			{
				ranking[c] += probs[b + D.getBinaryCount() / 2] / rc_count;
				comp_count[c]++;
			}
		}
	}
	
	NumberType sum = 0;
	for (int c = 0; c < D.getClassCount(); ++c)
	{
		ranking[c] /= comp_count[c];
		sum += ranking[c];
	}
				
	for (int c = 0; c < D.getClassCount(); ++c)
	{
		ranking[c] /= sum;
	}	
	
	delete [] comp_count;
	
	return ranking;
}

/*
			int* comp_count = new int[num_of_tops];
			int**         D = model->get_decomposition_matrix();
			
			for (int c=0; c<num_of_tops; c++)
			{
				ranking[c]    = 0.0;
				comp_count[c] = 0;
			}
			
			for (int b=0; b < num_of_binary_problems/2; b++)
			{//MLC_ranking[b][j] - вероятность 1 в b бинарной проблемме
				int oc_count = 0, rc_count = 0;
				
				for (int c = 0; c < num_of_tops; ++c)
				{
					if ( D[b][c] == ONLY_CLASS )
					{
						++oc_count;
					}
					else if ( D[b][c] == REST_CLASS )
						++rc_count;
				}
				
				for (int c = 0; c < num_of_tops; ++c)
				{
					//float both = MLC_ranking[b][j] * MLC_ranking[b + num_of_binary_problems/2][j];
					
					if ( D[b][c] == ONLY_CLASS )
					{
						ranking[c] += MLC_ranking[b][j] / oc_count;//both + * ( 1-MLC_ranking[b + num_of_binary_problems/2][j] );
						comp_count[c]++;
					}
					else if ( D[b][c] == REST_CLASS )
					{
						ranking[c] += MLC_ranking[b + num_of_binary_problems/2][j] / rc_count;//both + ( 1 - MLC_ranking[b][j] ) * 
						comp_count[c]++;
					}
				}
*/

/*
	for (int b=0; b < nr_binary/2; b++)
	{
		for (int c = 0; c < nr_class; ++c)
		{
			NumberType both = ( 1-probs[b] ) * ( 1-probs[b + nr_binary/2] );
						
			if ( D[b][c] == ONLY_CLASS )
			{
				ranking[c] += both + probs[b] * ( 1-probs[b + nr_binary/2] );
				comp_count[c]++;
			}
			else if ( D[b][c] == REST_CLASS )
			{
				ranking[c] += both + ( 1 - probs[b] ) * probs[b + nr_binary/2];
				comp_count[c]++;
			}
		}
	}
 */
