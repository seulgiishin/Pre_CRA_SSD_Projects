#pragma once
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

class CommandParser {
public:
	string cmd;
	int lba;
	int endLba;
	string data;
	int size;
	vector<string> result;

	void setResult(vector<string> result) {
		this->result = result;
	}

	vector<string> getResult() {
		return result;
	}

	void clearResult() {
		result.clear();
	}

	void command_parse(string cmd_line) {
		if (cmd_line == "")
			throw exception("No Command");

		vector<string> tokens;
		tokens.clear();
		size_t prev = 0;
		size_t cur = cmd_line.find(' ');
		while (cur != string::npos) {
			string substring = cmd_line.substr(prev, cur - prev);
			tokens.push_back(substring);
			prev = cur + 1;
			cur = cmd_line.find(' ', prev);
		}
		tokens.push_back(cmd_line.substr(prev, cur - prev));
		size_t token_n = tokens.size();
		if (token_n == 1) {
			cmd = tokens[0];
			if ((cmd == "exit") || (cmd == "help") || (cmd == "fullread") || (cmd == "testapp1") || (cmd == "testapp2") || (cmd == "flush")) {
			}
			else
				throw exception("INVALID COMMAND");
		}
		else if (token_n == 2) {
			cmd = tokens[0];
			if (cmd == "read") {
				if (isLBAint(tokens[1]) == false)
					throw exception("LBA out of range");
				if (isLBAinRange(tokens[1]) == false)
					throw exception("LBA out of range");
				lba = stoi(tokens[1]);
			}
			else if (cmd == "fullwrite") {
				if (isDataValid(tokens[1]) == false)
					throw exception("Data out of range");
				data = tokens[1];
			}
			else
				throw exception("INVALID COMMAND");
		}
		else if (token_n == 3) {
			cmd = tokens[0];
			if (cmd == "write") {
				if (isLBAint(tokens[1]) == false)
					throw exception("LBA out of range");
				if (isLBAinRange(tokens[1]) == false)
					throw exception("LBA out of range");
				lba = stoi(tokens[1]);

				if (isDataValid(tokens[2]) == false)
					throw exception("Data out of range");
				data = tokens[2];
			}
			else if (cmd == "erase") {
				if (isLBAinRange(tokens[1]) == false || isSizeValid(tokens[1], tokens[2]) == false)
					throw exception("Data out of range");
				lba = stoi(tokens[1]);
				size = stoi(tokens[2]);
			}
			else if (cmd == "erase_range") {
				if (isLBAinRange(tokens[1]) == false || isLBAinRange(tokens[2]) == false || (stoi(tokens[2]) < stoi(tokens[1])))
					throw exception("Data out of range");
				lba = stoi(tokens[1]);
				endLba = stoi(tokens[2]);
			}
			else
				throw exception("INVALID COMMAND");
		}
		else if (token_n > 3)
			throw exception("INVALID COMMAND");
	}

	bool isLBAint(string LBA_string) {
		if (LBA_string.length() > 2)
			return false;
		bool flag = true;
		for (int i = 0; i < LBA_string.length(); i++) {
			if (LBA_string[i] < '0' || LBA_string[i] > '9') {
				flag = false;
				break;
			}
		}
		return flag;
	}

	bool isLBAinRange(string check_lba) {
		if ((0 <= stoi(check_lba)) && (stoi(check_lba) < 100))
			return true;
		return false;
	}

	bool isDataValid(string check_data) {
		if (check_data.length() != 10)
			return false;
		if (check_data[0] == '0' && check_data[1] == 'x') {
			bool flag = true;
			for (int i = 2; i < 10; i++) {
				if ((('A' <= check_data[i]) && (check_data[i] <= 'F')) || (('0' <= check_data[i]) && (check_data[i] <= '9')))	continue;
				else {
					flag = false;
					break;
				}
			}
			if (flag == true)
				return true;
		}
		return false;
	}

	bool isSizeValid(string startLBA, string size) {
		if (stoi(size) >= 0 && stoi(size) <= 100 && (stoi(startLBA) + stoi(size) >= 0 && stoi(startLBA) + stoi(size) <= 100))
			return true;
		return false;
	}
};