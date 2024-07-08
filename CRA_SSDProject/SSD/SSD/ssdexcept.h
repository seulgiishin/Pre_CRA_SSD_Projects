#pragma once
#include <stdexcept>
#include <string>

using std::exception;
using std::string;

class ssd_exception : public exception {
private:
	string what_message;
public:
	ssd_exception(const string message) {
		what_message = message;
	}
	const char* what() const override
	{
		return what_message.c_str();
	}
};

