#pragma once
#include <iostream>
using namespace std;


class wrongAdrress_exception :public std::exception
{
public:
	const char* what() { return "�߸��� �ּ� ����"; }
};

class wrongValue_exception :public std::exception
{
public:
	const char* what() { return "�߸��� Value ����"; }
};