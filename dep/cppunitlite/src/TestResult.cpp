
#include "cppunitlite/TestResult.h"
#include "cppunitlite/Failure.h"

#include <sstream>
#include <iostream>


TestResult::TestResult ()
	: failureCount (0)
{
}


void TestResult::testsStarted () 
{
}


void TestResult::addFailure (const Failure& failure) {
	std::stringstream ss;
	ss << "Failure in test '" << failure.testName << "' :" << failure.message;
	std::cout << ss.str() << std::endl;
	std::cout.flush();
	failureCount++;
}


void TestResult::testsEnded ()  {
	std::stringstream ss;
	if (failureCount > 0) {
		ss << "There were " << failureCount << " failures";
	} else {
		ss << "There were no test failures";
	}
	std::cout << ss.str() << std::endl;
	std::cout.flush();
}
