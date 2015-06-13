#pragma once


#include "Failure.h"
#include "TestResult.h"
#include "Assertions.h"

#include <cmath>
#include <string>
#include <sstream>

class Test
{
public:
	Test (const char* testName, const char* group, int timeout);

	virtual void	runInternal () = 0;


	void			setNext(Test *test);
	Test			*getNext () const;
	void run(TestResult& result);
	
	const char* getName() {
		return name_.c_str();
	}

	const char* getGroup() {
		return group_.c_str();
	}
	
	int getTimeout() {
		return timeout_;
	}

protected:

	std::string		name_;
	std::string		group_;
	Test			*next_;
	int timeout_;

};

#define TEST(testName, testGroup, testTimeout)\
  class testGroup##testName##Test : public Test \
	{ public: testGroup##testName##Test () : Test (#testName , #testGroup , testTimeout) {} \
            void runInternal (); } \
    testGroup##testName##Instance; \
	void testGroup##testName##Test::runInternal() 



#define CHECK(msg, condition) { if (!(condition)) { Assertions::ConditionFailed(msg,#condition, __FILE__, __LINE__); \
	} \
}


#define ZSTR_EQUAL(msg,expected,actual) { \
	Assertions::StringEquals((msg), (expected), (actual), __FILE__, __LINE__); \
}

#define LONGS_EQUAL(msg,expected,actual)\
{ Assertions::LongEquals(msg,expected, actual, __FILE__, __LINE__); }

#define UINT32_EQUALS(msg,expected,actual)\
{ Assertions::UInt32Equals(msg,expected, actual, __FILE__, __LINE__); }

#define CHARS_EQUAL(msg,expected,actual)\
{ Assertions::CharEquals(msg,expected, actual, __FILE__, __LINE__); }

#define DOUBLES_EQUAL(msg, expected,actual,threshold)\
{ Assertions::DoubleEquals(msg,expected, actual, threshold, __FILE__, __LINE__); }

#define MEM_EQUAL(msg, expected,actual,size)\
{ Assertions::MemoryEquals(msg,expected, actual, size, __FILE__, __LINE__); }


#define FAIL(text) \
{ throw TestFailException(text, std::string(__FILE__), __LINE__);}

