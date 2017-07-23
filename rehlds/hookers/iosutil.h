#pragma once

#include <ios>

class IosFlagSaver {
public:
	explicit IosFlagSaver(std::ostream& _ios) :
		ios(_ios),
		f(_ios.flags()) {
	}
	~IosFlagSaver() {
		ios.flags(f);
	}

	IosFlagSaver(const IosFlagSaver &rhs) = delete;
	IosFlagSaver& operator= (const IosFlagSaver& rhs) = delete;

private:
	std::ostream& ios;
	std::ios::fmtflags f;
};
