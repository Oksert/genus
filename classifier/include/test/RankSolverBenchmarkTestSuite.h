#ifndef RANKSOLVERBENCHMARKTESTSUITE_H_
#define RANKSOLVERBENCHMARKTESTSUITE_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <ctime>

#include <iostream>

#include "test/TestData.h"

#include "rankSolver/SimpleTeamRankSolver.h"
#include "rankSolver/RankSolverTester.h"
#include "rankSolver/BTOneVsAllRankSolver.h"
#include "rankSolver/BTDGeneralRankSolver.h"

#include "rankSolver/probabilisticModel/SimpleProbabilisticModel.h"

using namespace std;

class RankSolverBenchmarkTestSuite : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(RankSolverBenchmarkTestSuite);
		CPPUNIT_TEST(testOne);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp()    {cout << endl;} //This will be executed before each test
	void tearDown() {cout << endl;} //This will be executed after each test
	
	void testOne()
	{
		int n = 3;
		
		int nrc = 20;
		
		SimpleTeamRankSolver strs;
		BTOneVsAllRankSolver btovars;
		BTDGeneralRankSolver btgeneral;

		DecompositionMatrix m1 = DecompositionMatrixGenerator::generateSparseMatrix(nrc, 0.8);
		DecompositionMatrix m2 = DecompositionMatrixGenerator::generateOneVsAllMatrix(nrc);
		
		vector<DecompositionMatrix*> D(n); D[0] = &m1; D[1] = &m2; D[2]=&m2;
		
		vector<AbstractRankSolver*> rs(n); rs[0] = &strs; rs[1] = &btovars; rs[2] = &btgeneral;
		
		vector<string> names(n);
		names[0] = "SimpleTeamRankSolver";
		names[1] = "BTOneVsAllRankSolver";
		names[2] = "BTDGeneralRankSolver";
		
		SimpleProbabilisticModel model;
		RankSolverTester tester(10, 10, model);
		
		map<string, vector<NumberType> > result = tester.testRankSolvers(rs, D, 1);
		
		cout << "Number of classs - " << nrc << endl;
		printResults(result, names);
	}
	
	void printResults(const map<string, vector<NumberType> >& r, const vector<string>& names)
	{
		int max_name_len = 0;
		
		for(int i=0; i < names.size(); ++i)
			if (names[i].size() > max_name_len)
				max_name_len = names[i].size();
		
		int max_property_size = 0;
		
		for (map<string, vector<NumberType> >::const_iterator it=r.begin() ; it != r.end(); it++ )
		{
			if ((*it).first.size() > max_property_size)
				max_property_size = (*it).first.size();
		}
		
		int last_width = cout.width(max_property_size+1); cout << right << " "; cout.width(max_name_len);
		
		for(int i=0; i < names.size(); ++i)
			cout << right << names[i] << " ";
		cout << endl;
		
		for (map<string, vector<NumberType> >::const_iterator it=r.begin() ; it != r.end(); it++ )
		{
			cout.width(max_property_size); cout << right << (*it).first << " ";
			
			for(int i=0; i < (*it).second.size(); ++i)
			{
				cout.width(max_name_len);
				cout << right << (*it).second[i] << " ";
			}
			
			cout << endl;
		}
		
		cout.width(last_width);
	}
};

#endif /*RANKSOLVERBENCHMARKTESTSUITE_H_*/
