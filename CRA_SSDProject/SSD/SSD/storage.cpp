#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <memory>
#include <windows.h>

#include "Logger.h"

#include "ssdexcept.h"
#include "util.h"

using namespace std;
using util::IntToHexUppercaseString;

using Range = std::pair<int, int>;

#define DEBUG (0)
#if DEBUG == 1
	constexpr eLoggingOpt LOGTYPE = eLoggingOpt::ALL_PRINT;
#else
	constexpr eLoggingOpt LOGTYPE = eLoggingOpt::ONLY_FILE;
#endif
#define LOGTAG string("STORAGE::")+util::GetClassMethodName(__FUNCSIG__)
#define LOGGER(x) Logger::GetInstance().write_Log(LOGTYPE, LOGTAG, x)

struct CommandSet
{
	int cmdOpcode;
	int address;
	int data;
	int size;
};

const enum DeviceType {
	TYPE_SSD,
};

interface Storage {
	virtual void write(int address, int data) = 0;
	virtual int read(int address) = 0;
	virtual void erase(int address, int size) = 0;
	virtual void flush() = 0;
};

class SSD : public Storage {
public:
	static const int COMMAND_WRITE = 0x1;
	static const int COMMAND_READ = 0x2;
	static const int COMMAND_ERASE = 0x3;
	static const int COMMAND_FLUSH = 0x4;

	void write(int address, int data) override {
		CommandSet cmd = { COMMAND_WRITE, address, data };

		checkingValidLba(address, 1);
		LOGGER("[LBA] Data : [" + to_string(address) + "] " + IntToHexUppercaseString(data) + " (Buffering)");

		setCommandList(cmd);
	} 

	int read(int address) override {
		checkingValidLba(address, 1);

		vector<CommandSet> cmdset = getCommandList();
		CommandSet cmd = { SSD::COMMAND_READ, address };

		int data;
		bool cachehit = getDataFromBuffer(cmd, data);
		if (cachehit)
		{
			LOGGER("[LBA] Data : [" + to_string(address) + "] " + IntToHexUppercaseString(data) + " (From Buffer)");
			return data;
		}

		data = getDataFromNand(address);
		LOGGER("[LBA] Data : [" + to_string(address) + "] " + IntToHexUppercaseString(data) + " (From Nand)");
		return data;
	}

	void erase(int address, int size) override {
		CommandSet cmd = { COMMAND_ERASE, address, 0, size };

		checkingValidLba(address, size);
		checkingEraseSize(size);

		LOGGER("[Start] ~ [End] : [" + to_string(address) + "] ~ [" + to_string(address + size - 1) + "] (Buffering)");
		setCommandList(cmd);
	}

	void flush() override {
		vector<CommandSet> cmdset;
		cmdset = getCommandList();

		for (CommandSet cmd : cmdset) {
			switch (cmd.cmdOpcode) {
			case COMMAND_WRITE:
				cmdWrite(cmd);
				break;
			case COMMAND_ERASE:
				cmdErase(cmd);
				break;
			default:
				throw ssd_exception("Unknown Command");
				break;
			}
		}
		DeleteFile(wstring(CMDFILE.begin(), CMDFILE.end()).c_str());

		LOGGER(string("Complete Flush (") + to_string(cmdset.size()) + " Commands)");
	}

private:
	const string OUTPUT = "result.txt";
	const string NAND = "nand.txt";
	const string CMDFILE = "cmdlist.txt";
	static constexpr int SSD_CAPACITY = 100;
	static constexpr int MIN_LBA = 0;
	static constexpr int MAX_LBA = (SSD_CAPACITY -1);
	static constexpr int MIN_ERASE_SIZE = 1;
	static constexpr int MAX_ERASE_SIZE = 10;
	static constexpr int CLEAN_PAGE_DATA = 0;

	void checkingValidLba(int address, int size)
	{
		if (address < MIN_LBA || (address + size - 1) > MAX_LBA)
		{
			string errorMessage = "Address range is ";
			errorMessage += std::to_string(MIN_LBA) + " <= address <= " + std::to_string(MAX_LBA);
			throw ssd_exception(errorMessage);
		}
	}

	void checkingEraseSize(int size)
	{
		if (size > MAX_ERASE_SIZE)
		{
			string errorMessage = "Erase size must be less than or equal to ";
			errorMessage += std::to_string(MAX_ERASE_SIZE);
			throw ssd_exception(errorMessage);
		}
		else if (size < MIN_ERASE_SIZE)
		{
			string errorMessage = "Erase size must be at least ";
			errorMessage += std::to_string(MIN_ERASE_SIZE);
			throw ssd_exception(errorMessage);
		}
	}

	void checkDataInit() {
		ifstream checkFile(NAND);

		if (checkFile.good()) {
			checkFile.close();
			return;
		}

		ofstream firstFile(NAND);
		for (int i = MIN_LBA; i <= MAX_LBA; i++) {
			firstFile << IntToHexUppercaseString(CLEAN_PAGE_DATA) << endl;
		}
		firstFile.close();
		checkFile.close();
	}

	vector<string> getSsdData() {
		vector<string> data;
		ifstream inFile(NAND);

		if (!inFile.is_open()) {
			throw ssd_exception("Cannot Open File");
		}

		string line;
		while (getline(inFile, line)) {
			data.push_back(line);
		}
		inFile.close();

		if (data.size() != SSD_CAPACITY) {
			throw ssd_exception("File Broken");
		}

		return data;
	}

	void setSsdData(vector<string> data) {
		ofstream outFile(NAND);

		if (!outFile.is_open()) {
			throw ssd_exception("Cannot Open File");
		}

		for (int i = MIN_LBA; i <= MAX_LBA; i++) {
			outFile << data[i] << endl;
		}

		outFile.close();
	}

	void writeResult(string value) {
		ofstream outFile(OUTPUT);

		if (!outFile.is_open()) {
			throw ssd_exception("Cannot Open File");
		}
		
		outFile << value << endl;

		outFile.close();
	}

	bool getDataFromBuffer(const CommandSet& cmd, int& data)
	{
		vector<CommandSet> cmdset = getCommandList();
		for (vector<CommandSet>::iterator it = cmdset.end(); it != cmdset.begin();)
		{
			it--;
			if (it->cmdOpcode == SSD::COMMAND_WRITE && it->address == cmd.address)
			{
				data = it->data;
				writeResult(IntToHexUppercaseString(data));
				return true;
			}
			int startlba = it->address;
			int endlba = it->address + it->size - 1;
			if (it->cmdOpcode == SSD::COMMAND_ERASE && (startlba <= cmd.address && cmd.address <= endlba))
			{
				data = it->data;
				writeResult(IntToHexUppercaseString(CLEAN_PAGE_DATA));
				return true;
			}
		}
		return false;
	}

	int getDataFromNand(int address)
	{
		checkDataInit();
		vector<string> ssdData = getSsdData();
		writeResult(ssdData[address]);

		return stoul(ssdData[address], nullptr, 16);
	}

	bool mergeOverlappedErase(vector<CommandSet> &commands) {
		bool isMerged = false;

		vector<Range> eraseRanges = makeEraseRangeSet(commands);
		if (eraseRanges.size() <= 1)
			return isMerged;

		vector<Range> result = mergeOverlappedRangeSet(eraseRanges);

		for (auto cmd = commands.begin(); cmd != commands.end();)
		{
			bool erasedRange = false;
			bool erasedCmd = false;
			if (cmd->cmdOpcode == COMMAND_ERASE)
			{
				for (std::vector<Range>::iterator range = result.begin(); range != result.end(); )
				{
					if (cmd->address == range->first)
					{
						cmd->size = range->second - range->first + 1;
						range = result.erase(range);
						erasedRange = true;
					}
					else
					{
						++range;
						
					}
				}
				if (erasedRange == false)
				{
					cmd = commands.erase(cmd);
					erasedCmd = true;
					isMerged = true;
				}
			}

			if (erasedCmd == false)
				++cmd;

		}

		return isMerged;
	}

	std::vector<Range> mergeOverlappedRangeSet(std::vector<Range>& eraseRanges)
	{
		std::vector<Range> result;
		sort(eraseRanges.begin(), eraseRanges.end());

		result.push_back(eraseRanges[0]);
		for (size_t i = 1; i < eraseRanges.size(); ++i) {
			if (result.back().second >= eraseRanges[i].first - 1) {
				result.back().second = max(result.back().second, eraseRanges[i].second);
			}
			else {
				result.push_back(eraseRanges[i]);
			}
		}
		return result;
	}

	std::vector<Range> makeEraseRangeSet(std::vector<CommandSet>& commands)
	{
		std::vector<Range> result;
		for (const auto& cmd : commands)
		{
			if (cmd.cmdOpcode == COMMAND_ERASE)
			{
				result.push_back({ cmd.address, cmd.address + cmd.size - 1 });
			}
		}
		return result;
	}

	bool mergeOverlappedWriteAndErase(vector<CommandSet>& commands) {
		bool isMerged = false;

		for (vector<CommandSet>::iterator cmd = std::prev(commands.end()); cmd > commands.begin();)
		{
			bool IsErasedWrite = false;
			if (cmd->cmdOpcode == COMMAND_ERASE || cmd->cmdOpcode == COMMAND_WRITE)
			{
				IsErasedWrite = scanPreviousWriteMergeSameRangeWriteErase(cmd, commands);
			}

			if (IsErasedWrite == false)
			{ 
				--cmd;
			}
			else
			{
				/*
				After an iterator becomes invalidated, restart the loop operation from the beginning to obtain valid iterators.
				While this approach ensures safe access, it may impact performance.
				*/
				cmd = std::prev(commands.end());
				isMerged = true;
			}
		}
		return isMerged;
	}

	bool scanPreviousWriteMergeSameRangeWriteErase(std::vector<CommandSet>::iterator& cmd, std::vector<CommandSet>& commands)
	{
		bool isMerged = false;
		int beginAddress = cmd->address;
		int endAddress = cmd->address;
		if (cmd->cmdOpcode == COMMAND_ERASE) endAddress = cmd->address + cmd->size - 1;
		else if (cmd->cmdOpcode == COMMAND_WRITE) endAddress = cmd->address;

		for (vector<CommandSet>::iterator cmdUnder = std::prev(cmd); cmdUnder >= commands.begin();)
		{
			if (cmdUnder->cmdOpcode == COMMAND_WRITE)
			{
				isMerged = mergePreviousWriteIfSameRange(beginAddress, cmdUnder, endAddress, commands);
			}

			if (cmdUnder == commands.begin())
				break;
			--cmdUnder;
		}
		return isMerged;
	}

	bool mergePreviousWriteIfSameRange(int beginAddress, std::vector<CommandSet>::iterator& cmdUnder, int endAddress, std::vector<CommandSet>& commands)
	{
		bool isMerged = false;
		if (beginAddress <= cmdUnder->address && cmdUnder->address <= endAddress)
		{
			cmdUnder = commands.erase(cmdUnder);
			isMerged = true;
		}
		return isMerged;
	}

	void saveCommandBuffer(vector<CommandSet> cmdlist) {
		ofstream cmdFile(CMDFILE);
		if (!cmdFile.is_open()) {
			throw ssd_exception("Cannot Open File");
		}

		for (CommandSet tempCmd : cmdlist) {
			string cmdStr;

			cmdStr = to_string(tempCmd.cmdOpcode) + " ";
			cmdStr += to_string(tempCmd.address) + " ";
			cmdStr += to_string(tempCmd.data) + " ";
			cmdStr += to_string(tempCmd.size);
			cmdFile << cmdStr << endl;
		}
		cmdFile.close();
	}

	void cmdWrite(CommandSet cmd) {
		vector<string> ssdData;
		int address = cmd.address;
		int data = cmd.data;
		
		LOGGER(string("Write(LBA: ") + to_string(cmd.address) + ", Data: " + IntToHexUppercaseString(data) + ") (To Nand)");

		checkDataInit();
		ssdData = getSsdData();
		ssdData[address] = IntToHexUppercaseString(data);
		setSsdData(ssdData);
	}

	void cmdErase(CommandSet cmd) {
		vector<string> ssdData;
		int address = cmd.address;
		int size = cmd.size;

		LOGGER(string("Erase(LBA: ") + to_string(cmd.address) + " ~ " + to_string(cmd.address + cmd.size - 1) + ") (To Nand)");

		checkDataInit();
		ssdData = getSsdData();
		for (int i = address; i < address + size; i++) {
			ssdData[i] = IntToHexUppercaseString(CLEAN_PAGE_DATA);
		}
		setSsdData(ssdData);
	}

	void setCommandList(CommandSet cmd) {
		vector<CommandSet> cmdlist;

		cmdlist = getCommandList();
		cmdlist.push_back(cmd);

		if (cmd.cmdOpcode == SSD::COMMAND_ERASE)
		{
			mergeOverlappedErase(cmdlist);
		}

		if (cmd.cmdOpcode == SSD::COMMAND_ERASE || cmd.cmdOpcode == SSD::COMMAND_WRITE)
		{
			mergeOverlappedWriteAndErase(cmdlist);
		}

		saveCommandBuffer(cmdlist);
		if (IsBufferFull(cmdlist)) {
			LOGGER(string("Command Buffering Auto Flush"));
			flush();
		}
	}

	bool IsBufferFull(std::vector<CommandSet>& cmdlist)
	{
		return cmdlist.size() >= 10;
	}

	vector<CommandSet> getCommandList() {
		ifstream cmdFile(CMDFILE);
		vector<CommandSet> cmdlist;

		if (!cmdFile.good()) {
			cmdFile.close();
			return cmdlist;
		}

		string line;
		while (getline(cmdFile, line)) {
			istringstream iss(line);
			CommandSet tempCmd;

			iss >> tempCmd.cmdOpcode >> tempCmd.address >> tempCmd.data >> tempCmd.size;
			cmdlist.push_back(tempCmd);
		}
		cmdFile.close();

		return cmdlist;
	}

};

interface Device {
	virtual Storage* setDevice(enum DeviceType) = 0;
};

class StorageDevice : public Device {
public:
	Storage* setDevice(enum DeviceType type) {
		if (type == TYPE_SSD) {
			return new SSD();
		}
		return nullptr;
	}

private:
	Storage* storage;
};
