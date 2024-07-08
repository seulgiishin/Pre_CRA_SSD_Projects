#include "pch.h"
#include "Logger.h"

#include<iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <io.h>

Logger::Logger() {
	DWORD process_id = GetCurrentProcessId();
	HANDLE process_handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id);
	if (process_handle) {
		char buffer[MAX_PATH] = {};
		DWORD buffer_size = MAX_PATH;
		QueryFullProcessImageNameA(process_handle, 0, buffer, &buffer_size);
		CloseHandle(process_handle);

		string processPath(buffer);
		processPath.erase(processPath.find_last_of("."), std::string::npos);
		auto idx = processPath.find_last_of('\\');
		processName = processPath.substr(idx, processPath.length() - idx);
	}
}

void Logger::write_Log(eLoggingOpt loggingOption, string functionName, string log_detail, bool addEndl) {
	string log = getCurrentTimetoString() + " " + setPaddingString(functionName + "()") + " : " + log_detail;
	if (addEndl && log_detail[log_detail.size()] != '\n')
		log = log + "\n";

	switch (loggingOption)
	{
	case eLoggingOpt::ALL_PRINT:
		printConsole(log);
		writeLogFile(log);
		break;
	case eLoggingOpt::ONLY_FILE:
		writeLogFile(log);
		break;
	case eLoggingOpt::ONLY_CONSOLE:
		printConsole(log);
		break;
	default:
		break;
	}
}

string Logger::getCurrentTimetoString() {
	auto chrono_Now = std::chrono::system_clock::now();
	auto millisec = chrono::duration_cast<chrono::milliseconds>(chrono_Now.time_since_epoch()) % 1000;

	time_t tm_now = chrono::system_clock::to_time_t(chrono_Now);
	now_time = new tm();
	localtime_s(now_time, &tm_now);

	char temp[128];
	strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S.", now_time);

	std::ostringstream currentTime_oss;
	currentTime_oss << "[" << temp << left << setfill('0') << setw(3) << millisec.count() << "]";
	return currentTime_oss.str();
}

string Logger::setPaddingString(const string& str)
{
	std::ostringstream funcNamePadding_oss;
	funcNamePadding_oss << left << setfill(' ') << setw(30) << str;
	return funcNamePadding_oss.str();
}

void Logger::printConsole(const string& log) {
	cout << log;
}

void Logger::writeLogFile(const string& log) {
	check_LogDir(logDir);

	string logPath = logDir+processName + "\\latest.log";
	ofstream logFile(logPath, ios::app);
	if (logFile.is_open()) {
		logFile << log;
	}
	logFile.close();

	check_Filesize(logPath);
}

void Logger::check_LogDir(std::string& logDir)
{
	filesystem::path dirPath(logDir);
	if (filesystem::exists(dirPath) == false) {

		if (create_directory(dirPath) == false)
			throw exception("Log Directory 생성 실패");
	}

	dirPath = logDir + processName;
	if (filesystem::exists(dirPath) == false) {

		if (create_directory(dirPath) == false)
			throw exception("Log\\[Process] Directory 생성 실패");
	}
}

void Logger::check_Filesize(std::string& logPath)
{
	auto fileSize = filesystem::file_size(logPath);
	// 파일 크기 체크 및 변환 구현 필요.
	if (fileSize > 10240)
	{
		press_PrevUntillLog();

		make_UntillLog(logPath);
	}
}

void Logger::press_PrevUntillLog()
{
	string prevFilePath = logDir + processName + "\\untill*.log";
	struct _finddata_t fd;
	intptr_t handle;
	if ((handle = _findfirst(prevFilePath.c_str(), &fd)) != -1L) {
		do {
			string prevFilename = logDir+processName + "\\" + fd.name;
			pressLogFile(prevFilename);

		} while (_findnext(handle, &fd) == 0);
		_findclose(handle);
	}
}

void Logger::pressLogFile(string prevFilename) {
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(prevFilename.c_str(), drive, dir, fname, ext);

	char chgFilename[_MAX_DIR];
	sprintf(chgFilename, "%s%s%s%s", drive, dir, fname, ".zip");

	filesystem::rename(prevFilename, chgFilename);
}

void Logger::make_UntillLog(std::string& logPath)
{
	string newName = logDir + processName;
	char temp[128];
	strftime(temp, sizeof(temp), "%Y%m%d %Hh%Mm%Ss", now_time);
	newName.append("\\untill_");
	newName.append(temp);
	newName.append(".log");
	filesystem::rename(logPath, newName);
}