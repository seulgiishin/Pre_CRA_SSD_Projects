#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;
class TestCase
{
public:
	TestCase(string fileName);
	~TestCase();
	string getId();
	bool execute(CommandParser &cp, CommandInvoker &invoker);
private:
	enum class Tag {
		ID,
		STEPS,
		EXPECTED,
	};
private:
	string mId;
	vector<string> mSteps;
	vector<string> mExpectedResults;
	string mFileName;
	static unordered_map<Tag, string> mTags;
	const string mTestCasePath = "testcases/";
};

