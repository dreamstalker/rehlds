#pragma once

#include <exception>
#include <sstream>

class TestFailException : public std::exception {
public:

	TestFailException(std::string message, std::string fileName, long lineNumber) {
		std::stringstream ss;
		ss << message << " at " << fileName << " line " << lineNumber;
		this->message = ss.str();
		this->fileName = fileName;
		this->lineNumber = lineNumber;
	}

	virtual ~TestFailException() throw() {

	}

	std::string message;
	std::string fileName;
	long lineNumber;

	virtual const char * what() const throw() {
		return message.c_str();
	}
};

class Failure {

public:
	Failure (TestFailException &e, std::string testName) {
		this->testName = testName;
		this->message = e.message;
		this->fileName = e.fileName;
		this->lineNumber = e.lineNumber;
	}

	Failure (std::string message, std::string testName) {
		this->testName = testName;
		this->message = message;
		this->fileName = "<unknown>";
		this->lineNumber = -1;
	}

	std::string testName;
	std::string message;
	std::string fileName;
	long lineNumber;
};




