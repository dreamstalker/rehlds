///////////////////////////////////////////////////////////////////////////////
//
// TESTREGISTRY.H
// 
// TestRegistry is a singleton collection of all the tests to run in a system.  
// 
///////////////////////////////////////////////////////////////////////////////

#pragma once

class Test;
class TestResult;



class TestRegistry
{
public:
	static void addTest (Test *test);
	static void runAllTests (TestResult& result);

	static Test* getFirstTest();
private:

	static TestRegistry&	instance ();
	void					add (Test *test);
	void					run (TestResult& result);

	
	Test					*tests;

};
