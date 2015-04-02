#include "decompositionMatrix/DecompositionMatrixGenerator.h"

#include <cmath>
#include <algorithm>

int DecompositionMatrixGenerator::dense[6]  = {3,3,10,10,20,20};
int DecompositionMatrixGenerator::sparse[3] = {3,10,30};

const float DecompositionMatrixGenerator::ZERO_PROB   = 0.5;
const int   DecompositionMatrixGenerator::ECOC_CYCLES = 100;

DecompositionMatrix DecompositionMatrixGenerator::generateOneVsAllMatrix(int nr_class)
{
	DecompositionMatrix D(2*nr_class, nr_class);
	
	for(int b=0; b < nr_class; b++)
	{
		for(int c=0; c < nr_class; c++)
			D[b][c] = -1;
		D[b][b] = 1;
	}
	
	for(int b=nr_class; b < 2*nr_class; b++)
	{
		for(int c=0; c < nr_class; c++)
			D[b][c] = 1;
		D[b][b-nr_class] = -1;
	}
	
	return D;
}

DecompositionMatrix DecompositionMatrixGenerator::generateOneVsOneMatrix(int nr_class)
{
	DecompositionMatrix D(nr_class*(nr_class-1), nr_class);
	
	int j=0, k=j+1;
	
	for (int b=0; b<D.getBinaryCount()/2; ++b)
	{
		int b_m = D.getBinaryCount()/2 + b;
		
		D[b][j]= ONLY_CLASS;
		D[b][k]= REST_CLASS;
		
		D[b_m][j]=REST_CLASS;
		D[b_m][k]=ONLY_CLASS;
		
		if ( k == (D.getClassCount()-1) ) 
		{
			j++; k=j+1;
		}
		else
			k++;
	}
	
	return D;
}

DecompositionMatrix DecompositionMatrixGenerator::generateDenseMatrix(int nr_class)
{
	if(nr_class < 9)
	{
		return generateDenseMatrix( dense[nr_class-3], nr_class );
	}
	else
	{//TODO change if needed
		//TODO round changed to (int)(x + 0.5)
		return generateDenseMatrix( (int)(nr_class * log((double)nr_class)/log(2.0) + 0.5), nr_class );
	}
}

DecompositionMatrix DecompositionMatrixGenerator::generateDenseMatrix(int nr_binary, int nr_class)
{
	DecompositionMatrix current(nr_binary, nr_class);
	DecompositionMatrix    best(nr_binary, nr_class);
	
	float ghd = 0.0;
	float max_ghd = 0.0;
	
	// Generate ECOC_CYCLES I and select the best one
	for(int n = 0; n < ECOC_CYCLES; n++)
	{
		generateDenseMatrix(current);
			
		if(!current.isCorrect())
		{
			n--;
			continue;
		}

		ghd = current.ghdScore();
		
		if(ghd > max_ghd)
		{
			max_ghd = ghd;
			best    = current;
		}
	}
	
	return DecompositionMatrix::makeMirror(best);
}

void DecompositionMatrixGenerator::generateDenseMatrix (DecompositionMatrix& d)
{
	int l1 = REST_CLASS, l2 = ONLY_CLASS;
	
	for(int i=0; i < d.nr_binary; i++)
	{
		for(int j=0; j < d.nr_class; j++)
			d.m[i][j] = l1;
	

		for(int j=0; j < d.nr_class/2; j++)
		{	
			int p = rand() % d.nr_class;
			while(d.m[i][p] == l2)
				p = rand() % d.nr_class;
			d.m[i][p] = l2;
		}
		
		std::swap(l1, l2);
	}
}

DecompositionMatrix DecompositionMatrixGenerator::generateSparseMatrix(int nr_class, float zeroProb)
{
	if(nr_class < 6)
	{
		return generateSparseMatrix( sparse[nr_class-3], nr_class, zeroProb );
	}
	else
	{//TODO change if needed
		//TODO changed round
		return generateSparseMatrix( (int)(nr_class * log((double)nr_class)/log(2.0) + 0.5), nr_class, zeroProb );
	}
}

DecompositionMatrix DecompositionMatrixGenerator::generateSparseMatrix(int nr_binary, int nr_class, float zeroProb)
{
	DecompositionMatrix current(nr_binary, nr_class);
	DecompositionMatrix    best(nr_binary, nr_class);
	
	float ghd = 0.0;
	float max_ghd = 0.0;
	
	// Generate ECOC_CYCLES I and select the best one
	for(int n = 0; n < ECOC_CYCLES; n++)
	{
		generateSparseMatrix(current, zeroProb);
			
		if(!current.isCorrect())
		{
			n--;
			continue;
		}

		ghd = current.ghdScore();
		
		if(ghd > max_ghd)
		{
			max_ghd = ghd;
			best    = current;
		}
	}
	
	return DecompositionMatrix::makeMirror(best);
}

void DecompositionMatrixGenerator::generateSparseMatrix(DecompositionMatrix& d, float zeroProb)
{//TODO make more determinable
	int f1 = 0, f2 = 0;
	
	for(int i=0; i < d.nr_binary; i++)
		do
		{
			f1=f2=0;
			for(int j=0; j < d.nr_class; j++)
			{
				double p = (rand()%1001)/1000.0;
				
				if(p > 0.5 + (zeroProb / 2) )
				{
					f1 = d.m[i][j] = ONLY_CLASS;
				}
				else if(p < 0.5 - (zeroProb / 2))
				{
					f2 = d.m[i][j] = REST_CLASS;
				}
				else
				{
					d.m[i][j] = ZERO_CLASS;
				}
			}
		}
		while(!(f1*f2));
}
