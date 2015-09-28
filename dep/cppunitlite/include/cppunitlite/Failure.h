#pragma once

#include <exception>
#include <sstream>

class TestFailException : public std::exception {
public:

	TestFailException(std::string message, std::string fileName, long lineNumber, bool onlyWarning = false) {
		std::stringstream ss;
		ss << message << " at " << fileName << " line " << lineNumber;
		this->message = ss.str();
		this->fileName = fileName;
		this->lineNumber = lineNumber;
		this->warning = onlyWarning;
	}

	virtual ~TestFailException() throw() {

	}

	std::string message;
	std::string fileName;
	long lineNumber;
	bool warning;

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
		this->warning = e.warning;
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
	bool warning;
};




