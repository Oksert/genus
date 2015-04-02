#ifndef RANKSOLVERTESTSUITE_H_
#define RANKSOLVERTESTSUITE_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>
#include <cmath>

#include "rankSolver/SimpleTeamRankSolver.h"
#include "rankSolver/BTOneVsAllRankSolver.h"
#include "rankSolver/BTDGeneralRankSolver.h"

#include "helper/VectorHelper.h"
#include "helper/MatrixHelper.h"

#include "test/TestData.h"
#include "decompositionMatrix/DecompositionMatrixGenerator.h"
#include "decompositionMatrix/DecompositionMatrix.h"

using namespace std;

class RankSolverTestSuite : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(RankSolverTestSuite);
		CPPUNIT_TEST(testSimpleTeamRankSolver);
		CPPUNIT_TEST(testBTOneVsAllRankSolver);
		CPPUNIT_TEST(testBTDGeneral);
		CPPUNIT_TEST(testBTDGeneralTime);
	CPPUNIT_TEST_SUITE_END();
	
protected:
	void outRanks(int nr_class, NumberType* r, char* intro)
	{
		cout << intro << " -";
		for(int c = 0; c < nr_class; ++c)
			cout << " " << c << " : " << r[c];
		cout <<  endl;
	}
	
//	static const NumberType eps;// = 0.001;
	
public:
	void setUp()    {cout << endl;} //This will be executed before each test
	void tearDown() {cout << endl;} //This will be executed after each test
	
	void testSimpleTeamRankSolver()
	{	
		DecompositionMatrix m = DecompositionMatrix::makeFrom2Darray((int*)d1, b1, c1);
		
		SimpleTeamRankSolver strs;
		
		NumberType* r = strs.solveRanks(m, p1, 0);
		
		/*for(int c = 0; c < c1; ++c)
			cout << " " << c << ":" << r[c];*/
		
		delete [] r;
	}
	
	void testBTOneVsAllRankSolver()
	{
		DecompositionMatrix m = DecompositionMatrix::makeFrom2Darray((int*)d2, b2, c2);
		
		BTOneVsAllRankSolver strs;
		
		for (int p=0; p < pc2; ++p)
		{
			NumberType* r = strs.solveRanks(m, (NumberType*)(p2[p]), 0);
			//outRanks(c2, r, "p2");
			
			//ѕроверить что итогова€ сумма равна единице
			// TODO eps = 0.001
			CPPUNIT_ASSERT( abs( VectorHelper::componentSum(c2, r) - 1) < 0.001 );
			
			//ѕроверить что сохранено пор€дковое соответствие между рангами и веро€тност€ми
			for(int f=0; f < c2-1; ++f)
				for (int s = f+1; s < c2; ++s)
				{
					if (p2[p][f] <= p2[p][s])
						CPPUNIT_ASSERT( r[f] <= r[s] );
					
					if (p2[p][f] >= p2[p][s])
						CPPUNIT_ASSERT( r[f] >= r[s] );
				}
			
			delete [] r;
		}
	}

	void testBTDGeneral()
	{
		DecompositionMatrix 	d			= DecompositionMatrix::makeFrom2Darray( (binLable*)d2, b2, c2);
		int						*docs_count	= new int[b2];
		BTDGeneralRankSolver	s;

		for(int b=0; b<b2; b++)
			docs_count[b] = 1;

		for (int p=0; p < pc2; ++p)
		{
			NumberType *r = s.solveRanks(d, (NumberType*)(p2[p]), docs_count);
			//outRanks(c2, r, "BTD result ");

			for( int c=0; c<c2; c++)
				CPPUNIT_ASSERT( fabs(r[c]-p2_answer[p][c]) < 0.01 );
			
			delete [] r;
		}
	}
	
	void testBTDGeneralTime()
	{
		NumberType* r;
		
		int nrc = 100;
		
//		timespec ts1, ts2, ts3, ts4;
		
		BTDGeneralRankSolver s;
		
		DecompositionMatrix d1 = DecompositionMatrixGenerator::generateOneVsAllMatrix(nrc);
		DecompositionMatrix d2 = DecompositionMatrixGenerator::generateOneVsOneMatrix(nrc);
		
		NumberType* probs = new NumberType[d2.getBinaryCount()];
		int*   docs_count = new int[d2.getBinaryCount()];
		
		for(int b = 0; b < d2.getBinaryCount(); ++b)
		{
			probs[b]      = (rand()%1001)/1001.0 + 0.0001;
			docs_count[b] = 1;
		}
		
//		clock_gettime(CLOCK_REALTIME, &ts1);
			r = s.solveRanks(d1, probs, docs_count);
//		clock_gettime(CLOCK_REALTIME, &ts2);
//		cout << "Time for One Vs All: " << (ts2.tv_nsec - ts1.tv_nsec) / 1000000.0 << " ms" << endl; //convert to milli seconds
		delete [] r;
		
//		clock_gettime(CLOCK_REALTIME, &ts3);
			r = s.solveRanks(d2, probs, docs_count);
//		clock_gettime(CLOCK_REALTIME, &ts4);
//		cout << "Time for One Vs One: " << (ts4.tv_nsec - ts3.tv_nsec) / 1000000.0 << " ms" << endl; //convert to milli seconds
		delete [] r;
		
		delete [] probs;
		delete [] docs_count;
	}

};

#endif /*RANKSOLVERTESTSUITE_H_*/
