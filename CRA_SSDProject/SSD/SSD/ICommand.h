#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>

#include "ssdexcept.h"
#include "storage.cpp"

using namespace std;

interface ICommand {
public:
	static const int COMMAND_VALIDATION_SUCCESS = 0;
	static const int COMMAND_VALIDATION_FAIL = -1;

	virtual string getCommandCode() const = 0;
	virtual string getHelpMessage() const = 0;
	virtual int execute(int argc, char* argv[], Storage* storage) = 0;

protected:
	bool IsValidData(const string value) {
		if (value.size() != 10)
			return false;

		std::regex reg("0x[0-9a-fA-F]{8}$");
		if (!regex_match(value, reg))
			return false;

		return true;
	}

	bool isDecimal(const std::string& str) {
		if (str.empty()) return false;

		for (char c : str) {
			if (!(std::isdigit(c))) {
				return false;
			}
		}

		return true;
	}

	int checkParameter(int argc, char* argv[], CommandSet& commandset) {

		if (argc == 2 && string(argv[1]) == "F")
		{
			commandset.cmdOpcode = SSD::COMMAND_FLUSH;
			commandset.address = -1;
			commandset.data = -1;
			commandset.size= -1;
		}
		else if (argc == 3 && string(argv[1]) == "R")
		{
			if (!isDecimal(string(argv[2])))
			{
				throw ssd_exception(string("[Argument Validation] Address is invalid: ") + argv[2]);
			}
			commandset.cmdOpcode = SSD::COMMAND_READ;
			commandset.address = std::stoi(string(argv[2]));
			commandset.data = -1;
			commandset.size = -1;
		}
		else if (argc == 4 && string(argv[1]) == "W")
		{
			
			if (!isDecimal(string(argv[2])))
			{
				throw ssd_exception(string("[Argument Validation] Address is invalid: ") + argv[2]);
			}
			if (!IsValidData(string(argv[3])))
			{
				throw ssd_exception(string("[Argument Validation] Data is invalid: ") + argv[3]);
			}
			commandset.cmdOpcode = SSD::COMMAND_WRITE;
			commandset.address = std::stoi(string(argv[2]));
			commandset.data = static_cast<int>(std::stoll(string(argv[3]), nullptr, 16));
			commandset.size = -1;
		}
		else if (argc == 4 && string(argv[1]) == "E")
		{
			if (!isDecimal(string(argv[2])))
			{
				throw ssd_exception(string("[Argument Validation] Address is invalid: ") + argv[2]);
			}
			if (!isDecimal(string(argv[3])))
			{
				throw ssd_exception(string("[Argument Validation] Size is invalid: ") + argv[2]);
			}
			commandset.cmdOpcode = SSD::COMMAND_ERASE;
			commandset.address = std::stoi(string(argv[2]));
			commandset.data = -1;
			commandset.size = std::stoi(string(argv[3]));
		}
		else
			return COMMAND_VALIDATION_FAIL;

		return COMMAND_VALIDATION_SUCCESS;
	}
};
