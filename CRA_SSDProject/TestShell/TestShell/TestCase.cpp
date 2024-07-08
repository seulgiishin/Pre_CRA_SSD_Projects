#include <iostream>
#include <fstream>
#include "CommandParser.cpp"
#include "invoker.cpp"
#include "Runner.cpp"
#include "TestCase.h"
#include "json.hpp"

using json = nlohmann::json;

unordered_map<TestCase::Tag, string> TestCase::mTags = {
	   {Tag::ID, string("ID")},
	   {Tag::STEPS, string("STEPS")},
	   {Tag::EXPECTED, string("EXPECTED")},
};

TestCase::TestCase(string fileName) : mFileName(fileName) {
	json jsonParser;
	string name = mTestCasePath;
	name.append(fileName);
	name.append(".tc");
	std::ifstream file(name, std::ios::in);
	if (file.is_open()) {
		file >> jsonParser;
		file.close();
	}
	mId = jsonParser[mTags.at(Tag::ID)];

	for (const auto& item : jsonParser[mTags.at(Tag::STEPS)]) {
		mSteps.push_back(item);
	}

	int i = 0;
	for (const auto& item : jsonParser[mTags.at(Tag::EXPECTED)]) {
		string expectFileName = mTestCasePath;
		expectFileName.append(item);
		std::ifstream expectedFile(expectFileName, std::ios::in);
		if (expectedFile.is_open()) {
			string expected;
			string buf;
			while (getline(expectedFile, buf)) {
				expected.append(buf);
			}
			mExpectedResults.push_back(expected);
			expectedFile.close();
		}
	}
}
TestCase::~TestCase() {
	mSteps.clear();
	mExpectedResults.clear();
}

string TestCase::getId() {
	return mId;
}

bool TestCase::execute(CommandParser& cp, CommandInvoker& invoker) {
	Logger& logger = Logger::GetInstance();

	logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, getId() + " --- Run...");
	cout << getId() << " --- Run...";
	try {
		int32_t expectIdx{};
		for (auto& step: mSteps) {
			cp.command_parse(step);
			transform(cp.cmd.begin(), cp.cmd.end(), cp.cmd.begin(), ::toupper);
			invoker.execute(cp);
			vector<string> actuals = cp.getResult();
			if (actuals.size() > 0) {
				string actualResult;
				for (const auto r : actuals) {
					actualResult.append(r);
				}
				if (actualResult != mExpectedResults[expectIdx++]) {
					logger.write_Log(eLoggingOpt::ONLY_FILE, "", "[FAIL]");
					cout << "[FAIL]" << endl;
					return false;
				}
				cp.clearResult();
			}
		}
		logger.write_Log(eLoggingOpt::ONLY_FILE, "", "[PASS]");
		cout << "[PASS]" << endl;
	}
	catch (exception) {
		logger.write_Log(eLoggingOpt::ONLY_FILE, "", "[FAIL]");
		cout << "[FAIL]" << endl;
		return false;
	}

	return true;
}