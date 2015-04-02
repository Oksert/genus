#ifndef DECOMPOSITIONMATRIXGENERATORTESTSUITES_H_
#define DECOMPOSITIONMATRIXGENERATORTESTSUITES_H_

#include "decompositionMatrix/DecompositionMatrixGenerator.h"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

using namespace std;

class DecompositionMatrixGeneratorTestSuite : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(DecompositionMatrixGeneratorTestSuite);
		CPPUNIT_TEST(testDense);
		CPPUNIT_TEST(testSparce);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp() { cout << endl; }   //This will be executed before each test

	void tearDown() { cout << endl; }//This will be executed after each test

	void testDense()
	{
		int nr_class = 5;
		
		DecompositionMatrix d = DecompositionMatrixGenerator::generateDenseMatrix(nr_class);
		
		cout << d;
		
		cout << "Number of binary - " << d.getBinaryCount();
	}
	
	void testSparce()
	{
		int nr_class = 10;
		
		DecompositionMatrix d = DecompositionMatrixGenerator::generateSparseMatrix(nr_class, 0.8);
		
		cout << d;
		
		cout << "Number of binary - " << d.getBinaryCount();
	}
};

#endif /*DECOMPOSITIONMATRIXGENERATORTESTSUITES_H_*/
