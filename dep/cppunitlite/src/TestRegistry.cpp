

#include "cppunitlite/Test.h"
#include "cppunitlite/TestResult.h"
#include "cppunitlite/TestRegistry.h"


void TestRegistry::addTest (Test *test) 
{
	instance ().add (test);
}


void TestRegistry::runAllTests (TestResult& result) 
{
	instance ().run (result);
}

Test* TestRegistry::getFirstTest() {
	return instance().tests;
}
	
TestRegistry& TestRegistry::instance () 
{
	static TestRegistry registry;
	return registry;
}


void TestRegistry::add (Test *test) 
{
	if (tests == 0) {
		tests = test;
		return;
	}
	
	test->setNext (tests);
	tests = test;
}


void TestRegistry::run (TestResult& result) 
{
	result.testsStarted ();

	for (Test *test = tests; test != 0; test = test->getNext ())
		test->run (result);
	result.testsEnded ();
}

