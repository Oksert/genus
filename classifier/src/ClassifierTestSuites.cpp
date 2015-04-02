#include "test/ClassifierTestSuites.h"

#include <iostream>
#include <cppunit/extensions/HelperMacros.h>
#include <cstdlib>

/*!Here should be included all test suites as this sample suite*/

//#include "test/SimpleTestSuite.h"
//CPPUNIT_TEST_SUITE_REGISTRATION(SimpleTestSuite);

#include "test/DecompositionMatrixGeneratorTestSuite.h"
CPPUNIT_TEST_SUITE_REGISTRATION(DecompositionMatrixGeneratorTestSuite);

#include "test/RankSolverTestSuite.h"
CPPUNIT_TEST_SUITE_REGISTRATION(RankSolverTestSuite);

#include "test/RankSolverBenchmarkTestSuite.h"
CPPUNIT_TEST_SUITE_REGISTRATION(RankSolverBenchmarkTestSuite);

/*!End of tests suites registaration region*/

void initializeClassifierTestSuites()
{
	std::cout << "Initializing classifier tests suites ..." << std::endl;
	return;	
}
