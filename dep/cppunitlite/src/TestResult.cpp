
#include "cppunitlite/TestResult.h"
#include "cppunitlite/Failure.h"

#include <sstream>
#include <iostream>


TestResult::TestResult ()
	: failureCount (0), warningCount (0)
{
}


void TestResult::testsStarted () 
{
}

void TestResult::addFailure (const Failure& failure) {
	std::stringstream ss;
	ss << (failure.warning ? "Warning in test '" : "Failure in test '") << failure.testName << "' :" << failure.message;
	std::cout << ss.str() << std::endl;
	std::cout.flush();

	if (failure.warning) {
		warningCount++;
	}
	else
		failureCount++;
}


void TestResult::testsEnded ()  {
	std::stringstream ss;
	if (failureCount > 0) {
		ss << "There were " << failureCount << " failures";
		if (warningCount > 0) {
			ss << ", and " << warningCount << " warnings";
		}
	}
	else if (warningCount > 0) {
		ss << "There were " << warningCount << " warnings";
	}
	else {
		ss << "There were no test failures";
	}

	std::cout << ss.str() << std::endl;
	std::cout.flush();
}
