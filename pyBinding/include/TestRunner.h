#ifndef TESTRUNNER_H_
#define TESTRUNNER_H_

#include <test/ClassifierTestSuites.h>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <iostream>

namespace CModule
{
	int runTests()
	{
		initializeClassifierTestSuites();
		//TODO Add more test suites initializers as it needed
		
		CPPUNIT_NS::TestResult controller;

		// Add a listener that colllects test result
		CPPUNIT_NS::TestResultCollector result;
		controller.addListener( &result );        

		// Add a listener that print dots as test run.
		CPPUNIT_NS::BriefTestProgressListener progress;
		controller.addListener( &progress );      

		// Add the top suite to the test runner
		CPPUNIT_NS::TestRunner runner;
		runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
		runner.run( controller );

		// Print test in a compiler compatible format.
		CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
		outputter.write(); 

		return 0;
	}
}

#endif /*TESTRUNNER_H_*/
