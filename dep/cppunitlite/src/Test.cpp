

#include "cppunitlite/Test.h"
#include "cppunitlite/TestRegistry.h"
#include "cppunitlite/Failure.h"

#include <exception>
#include <sstream>


Test::Test (const char* testName, const char* testGroup, int timeout) 
	: name_ (testName), group_ (testGroup), timeout_(timeout)
{
	next_ = NULL;
	TestRegistry::addTest(this);
}


Test *Test::getNext() const
{
	return next_;
}


void Test::setNext(Test *test)
{	
	next_ = test;
}

void Test::run(TestResult &result) {
	try {
		runInternal();
	} catch (TestFailException *e) {
		result.addFailure(Failure(*e, name_));
	} catch (std::exception &e) {
		std::stringstream ss;
		ss << "unexpected exception " << e.what();
		result.addFailure(Failure(ss.str(), name_));
	} catch (...) {
		result.addFailure(Failure("unknown exception", name_));
	}
}
