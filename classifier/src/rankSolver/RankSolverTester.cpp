#include "rankSolver/RankSolverTester.h"

#include <cmath>
#include <ctime>
#include <algorithm>

#include "helper/VectorHelper.h"
#include "helper/MatrixHelper.h"

using std::pair;

map<string, vector<NumberType> > RankSolverTester::testRankSolvers
	(const vector<AbstractRankSolver*>& rs, const vector<DecompositionMatrix*>& D, const vector<int*>& docs_count) const
{
//	timespec ts1, ts2;
	
	int n = rs.size();
	
	NumberType* ranks;
	NumberType* probs_b;
	
	model.init(D[0]->getClassCount());
	
	map<string, vector<NumberType> > result;
	
	vector<string> qpnames = model.getQualityParametrs();
	
	NumberType*** estimates = new NumberType**[times];
	for(int i=0; i < times; ++i)
	{
		estimates[i] = new NumberType*[n];
		for(int j=0; j < n; ++j)
			estimates[i][j] = new NumberType[qpnames.size() + 1]; // 1 for time
	}

	for(int t=0; t<times; ++t)
	{
		int id = model.generateProbabilies();
		
		for(int i=0; i<n; ++i)
		{
			probs_b = model.generateBinaryProbabilies(*D[i], id);
			
			VectorHelper::addNoise(D[i]->getBinaryCount(), probs_b, noise);
			
//			clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts1);
				ranks   = rs[i]->solveRanks(*D[i], probs_b, docs_count[i]);
//			clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts2);
			
			// TODO Portable time measurement
//			estimates[t][i][qpnames.size()] = (ts2.tv_nsec - ts1.tv_nsec) / 1000000.0; //convert to milli seconds
			
			model.estimateQuality(ranks,id,estimates[t][i]);
			
			delete [] ranks;
			delete [] probs_b;
		}
		
		model.releaseID(id);
	}
	
	vector<NumberType> e(n);
	vector<NumberType> v(n);
	
	qpnames.push_back("Time (ms)");
	
	//Calculating mean and variation
	for(int i=0; i < qpnames.size(); ++i)
	{
		//estimates[t][j][i]: t - number of simulation, j - number of rankSolver, i - number of parametr
		
		//from http://mathworld.wolfram.com/SampleVarianceComputation.html
		/*for(int j=0; j < n; ++j)
		{
			e[j] = estimates[0][j][i];
		}
		
		for(int j=0; j < n; ++j)
			for(int t = 1; t < times; ++t)
			{
				e[j] = t * (e[j]/(t+1)) + estimates[t][j][i] / (t+1);
			}
		
		for(int j=0; j < n; ++j)
			v[j] = fabs( e[j] - estimates[0][j][i] );
		
		for(int j=0; j < n; ++j)
			for(int t = 1; t < times; ++t)
			{
				v[j] = t * (v[j]/(t+1)) + fabs( e[j] - estimates[t][j][i] ) / (t+1);
			}*/
		
		
		//from http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance (Algorithm III)
		for(int j=0; j < n; ++j)
		{
			e[j] = 0;
			v[j] = 0;
		}
		
		for(int j=0; j < n; ++j)
			for(int t = 0; t < times; ++t)
			{
				NumberType delta = estimates[t][j][i] - e[j];
				
				e[j] += delta / (t + 1);
				
				v[j] += delta * (estimates[t][j][i] - e[j]);
			}
		
		for(int j=0; j < n; ++j)
			v[j] = sqrt(v[j] / (times - 1));
		
		
		//using GNU Scientific Library
		/*double* data = new double[times];
		for(int j=0; j < n; ++j)
		{
			for(int t = 0; t < times; ++t)
				data[t] = estimates[t][j][i];
			
			e[j] = gsl_stats_mean(data,1,times);
			v[j] = gsl_stats_sd(data,1,times);
		}
		delete [] data;*/
		
		result.insert( pair<string, vector<NumberType> >(qpnames[i] + " mean",e) );
		result.insert( pair<string, vector<NumberType> >(qpnames[i] + " sd",v) );
	}
	
	vector<NumberType> num_of_bin(n);
	for(int i=0; i<n; ++i)
	{
		num_of_bin[i] = D[i]->getBinaryCount();
	}
	result.insert( pair<string, vector<NumberType> >("Number of binary",num_of_bin) );
	
	for(int i=0; i < times; ++i)
	{
		for(int j=0; j < n; ++j)
			delete [] estimates[i][j];
		delete [] estimates[i];
	}
	delete [] estimates;
	
	return result;
}

map<string, vector<NumberType> > RankSolverTester::testRankSolvers
	(const vector<AbstractRankSolver*>& rs, const vector<DecompositionMatrix*>& D, int Docs_count) const
{
	int n = rs.size();
	
	int max = MatrixHelper::getMaxNumberOfBinary(D);
	
	int* docs_count = new int[max];
	for (int i=0; i<max; ++i)
		docs_count[i] = Docs_count;
	
	vector<int*> param(n);
	for (int i=0; i<n; ++i)
		param[i] = docs_count;
	
	map<string, vector<NumberType> > result = testRankSolvers(rs, D, param);
	
	delete [] docs_count;
	
	return result;
}
