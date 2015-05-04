#pragma once

class Failure;

class TestResult
{
public:
					TestResult (); 
	virtual void	testsStarted ();
	virtual void	addFailure (const Failure& failure);
	virtual void	testsEnded ();

	int getFailureCount() {
		return failureCount;
	}
private:
	int				failureCount;
};
