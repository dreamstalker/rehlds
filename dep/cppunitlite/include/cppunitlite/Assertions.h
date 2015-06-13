#pragma once

#include "Failure.h"

class Assertions {
public:
	static void StringEquals(std::string message, std::string expected, std::string actual, const char* fileName, long lineNumber);

	static void StringEquals(std::string message, const char* expected, const char* actual, const char* fileName, long lineNumber);

	static void ConditionFailed(std::string message, std::string condition, const char* fileName, long lineNumber);

	static void LongEquals(std::string message, long expected, long actual, const char* fileName, long lineNumber);

	static void UInt32Equals(std::string message, unsigned int expected, unsigned int actual, const char* fileName, long lineNumber);

	static void CharEquals(std::string message, char expected, char actual, const char* fileName, long lineNumber);

	static void DoubleEquals(std::string message, double expected, double actual, double epsilon, const char* fileName, long lineNumber);

	static void MemoryEquals(std::string message, void* expected, void* actual, int size, const char* fileName, long lineNumber);
};
