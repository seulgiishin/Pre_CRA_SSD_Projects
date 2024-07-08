#pragma once
#include <iostream>
using namespace std;


class wrongAdrress_exception :public std::exception
{
public:
	const char* what() { return "肋给等 林家 立辟"; }
};

class wrongValue_exception :public std::exception
{
public:
	const char* what() { return "肋给等 Value 屈侥"; }
};