#ifndef SIMPLETESTSUITE_H_
#define SIMPLETESTSUITE_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

using namespace std;

class SimpleTestSuite : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SimpleTestSuite);
		CPPUNIT_TEST(testOne);
		CPPUNIT_TEST(testAnother);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp()
	{//This will be executed before each test
		cout << endl;
	}

	void tearDown()
	{//This will be executed after each test
		cout << endl;
	}

	void testOne()
	{
		cout << "testOne sample output";
		int i = 5; 
		++i;  
	}
	
	void testAnother()
	{
		cout << "testAnother sample output";
		CPPUNIT_ASSERT(1==1);
	}
};

#endif /*SIMPLETESTSUITE_H_*/
