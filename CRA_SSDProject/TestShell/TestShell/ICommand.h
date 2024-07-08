#pragma once
#include <string>
#include <regex>
#include <stdexcept>
#include "CommandParser.cpp"
#include "ProductInterface.h"
#include "TestShell_Exception.h"
#include "Logger.h"

using namespace std;

class ICommand {
public:
	virtual string getCommand() = 0;
	virtual void execute(CommandParser& cp, IProduct* product) = 0;
	void setRunnerMode(bool run_flag) {
		if (isRunningTestScenario = run_flag)
			logOption = eLoggingOpt::ONLY_FILE;
		else
			logOption = eLoggingOpt::ALL_PRINT;
	}

	eLoggingOpt getLoggingMode() {
		return logOption;
	}

protected:
	void checkInvalid_LBA(int arr) {
		if (arr < MIN_LBA || arr > MAX_LBA)
			throw wrongAdrress_exception();
	}

	void checkInvalid_Value(string value) {
		std::regex reg("0x[0-9A-F]{8}$");
		if (!regex_match(value, reg))
			throw wrongValue_exception();
	}

	const int MIN_LBA = 0;
	const int MAX_LBA = 100;
	const string TEST_PATTERN_TESTAPP1 = "0xAABBCCDD";
	const string TEST_PATTERN_TESPAPP2 = "0x12345678";
	Logger& logger = Logger::GetInstance();
	bool isRunningTestScenario = false;
	eLoggingOpt logOption = eLoggingOpt::ALL_PRINT;
};
