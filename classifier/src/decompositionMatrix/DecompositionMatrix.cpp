#include "decompositionMatrix/DecompositionMatrix.h"

#include <cstdlib>

const float DecompositionMatrix::BIN_SEP_FACTOR = 2.0;

DecompositionMatrix DecompositionMatrix::makeMirror(const DecompositionMatrix& d)
{
	DecompositionMatrix result;
	
	result.nr_class  = d.nr_class;
	result.nr_binary = d.nr_binary * 2;
	
	result.m = new binLable*[result.nr_binary];
	
	for(int b = 0; b < d.nr_binary; b++)
	{
		result.m[b]               = new binLable[result.nr_class];
		result.m[b + d.nr_binary] = new binLable[result.nr_class];
		
		for(int c = 0; c < d.nr_class; c++)
		{
			result.m[b][c] = d.m[b][c];
			
			if (d.m[b][c] == ONLY_CLASS)
				result.m[b+d.nr_binary][c] = REST_CLASS;
			else if (d.m[b][c] == REST_CLASS)
				result.m[b+d.nr_binary][c] = ONLY_CLASS;
			else
				result.m[b+d.nr_binary][c] = ZERO_CLASS;
		}
	}
	
	return result;
}

DecompositionMatrix DecompositionMatrix::makeFrom2Darray(binLable* m, int nr_binary, int nr_class)
{
	DecompositionMatrix d;
	
	d.nr_class  = nr_class;
	d.nr_binary = nr_binary;
	
	d.m = new binLable*[nr_binary];
	
	for (int b=0; b<nr_binary; ++b)
	{
		d.m[b] = new binLable[nr_class];
		
		for(int c=0; c < nr_class; ++c)
			d.m[b][c] = m[b*nr_class + c];
	}
	
	return d;
}

DecompositionMatrix::DecompositionMatrix(int nr_binary, int nr_class)
{
	this->nr_class  = nr_class;
	this->nr_binary = nr_binary;
	
	m = new binLable*[nr_binary];
	
	for(int b=0; b < nr_binary; ++b)
	{
		m[b] = new binLable[nr_class];
		
		for(int c=0; c < nr_class; ++c)
			m[b][c] = ZERO_CLASS;
	}
}

DecompositionMatrix::DecompositionMatrix(const DecompositionMatrix& d)
{
	nr_class  = d.nr_class;
	nr_binary = d.nr_binary;
	
	m = new binLable*[nr_binary];
	
	for(int b=0; b < nr_binary; ++b)
	{
		m[b] = new binLable[nr_class];
		
		for(int c=0; c < nr_class; ++c)
			m[b][c] = d.m[b][c];
	}
}

DecompositionMatrix::DecompositionMatrix(binLable** m, int nr_binary, int nr_class)
{
	this->nr_class  = nr_class;
	this->nr_binary = nr_binary;

	this->m = new binLable*[nr_binary];
	
	for(int b=0; b < nr_binary; ++b)
	{
		m[b] = new binLable[nr_class];
		
		for(int c=0; c < nr_class; ++c)
			this->m[b][c] = m[b][c];
	}
}

DecompositionMatrix& DecompositionMatrix::operator = (const DecompositionMatrix& d)
{
	this->~DecompositionMatrix();

	nr_class  = d.nr_class;
	nr_binary = d.nr_binary;
	
	m = new binLable*[nr_binary];
	
	for(int b=0; b < nr_binary; ++b)
	{
		m[b] = new binLable[nr_class];
		
		for(int c=0; c < nr_class; ++c)
			m[b][c] = d.m[b][c];
	}
	//how this compiled before?
	return *this;
}

int DecompositionMatrix::isCorrect() const
{
	int c,i,j,k,s,z;

	// check for identical/complementary rows 
	for(i=0; i<nr_binary-1; i++)
		for(j=i+1; j<nr_binary; j++)
		{
			s = c = 0;
			for(k=0;k<nr_class;k++)
			{
				if(m[i][k] == m[j][k]) s++;
				if(m[i][k]+m[j][k] == 0) c++;
			}
			
			if(s==nr_class || c==nr_class)
				return 0;
		}
	
	// check for zero column
	for(i=0; i<nr_class; i++)
	{
		z = 0;
		for(j=0; j<nr_binary; j++)
			if(m[j][i] == 0) z++;
		if(z == nr_binary)
			return 0;
	}
	
	return 1;
}

DecompositionMatrix::~DecompositionMatrix()
{
	for(int b=0; b < nr_binary; ++b)
		delete [] m[b];
	
	delete [] m;
}

float DecompositionMatrix::ghdScore() const
{
	return ghdScore(BIN_SEP_FACTOR);
}

float DecompositionMatrix::ghdScore(float binSepFactor) const
{
	float class_sep = 0.0, bin_sep = 0.0;
	
	for (int s = 0; s < nr_class; ++s)
		for(int e = s+1; e < nr_class; ++e)
			for (int i=0; i<nr_binary; ++i)
				class_sep += abs( m[i][s]-m[i][e] ) / 2.0;
	
	for (int s = 0; s < nr_binary; ++s)
		for(int e = s+1; e < nr_binary; ++e)
			for (int i=0; i<nr_class; ++i)
				bin_sep   += abs( m[s][i]-m[e][i] ) / 2.0;
	
	return binSepFactor * bin_sep + (class_sep / nr_class) * nr_binary;
}

std::ostream& operator << (std::ostream& os, const DecompositionMatrix& d)
{
	os << std::endl;
	
	for (int c = 0; c < d.nr_class; ++c)
	{
		for (int b = 0; b < d.nr_binary; ++b)
			if ( d.m[b][c] == -1 )
				os << d.m[b][c];
			else
				os << " " << d.m[b][c];
		
		os << std::endl;
	}
	
	return os;
}
