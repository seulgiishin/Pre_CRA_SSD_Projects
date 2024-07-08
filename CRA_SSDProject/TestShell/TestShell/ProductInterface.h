#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "Logger.h"
using namespace std;



__interface IProduct
{
	virtual void Write(int addr, string value) = 0;
	virtual string Read(int addr) = 0;
	virtual void Erase(int addr, int size) = 0;
	virtual void Flush() = 0;
};

class SSDProduct : public IProduct
{
public:
	SSDProduct() {
		GetCurrentDirectoryA(256, curPath);
	}
	virtual void Write(int addr, string value) override {		
		string cmd = curPath;
		cmd.append(mExecuteName + " W " + std::to_string(addr) + " " + value);
		int res = system(cmd.c_str());

		if (res != 0)
			logger.write_Log(eLoggingOpt::ALL_PRINT, __FUNCTION__, "실행파일을 실행하지 못했습니다. : " + to_string(res) + "-" + cmd);
	}

	virtual string Read(int addr) override {
		string cmd = curPath;
		cmd.append(mExecuteName + " R " + std::to_string(addr));
		int res = system(cmd.c_str());

		if (res != 0)
			logger.write_Log(eLoggingOpt::ALL_PRINT, __FUNCTION__, "실행파일을 실행하지 못했습니다. : " + to_string(res) + "-" + cmd);


		ifstream readFile;
		string result;
		readFile.open(mReadFileName);
		if (readFile.is_open()) {
			string buf;
			while (getline(readFile, buf)) {
				result.append(buf);
			}
		}
		else
			logger.write_Log(eLoggingOpt::ALL_PRINT, __FUNCTION__, "결과 파일을 읽지 못했습니다. : " + cmd);
		readFile.close();
		return result;
	}

	virtual void Erase(int addr, int size) override {
		string cmd = curPath;
		cmd.append(mExecuteName + " E " + std::to_string(addr) + " " + std::to_string(size));
		int res = system(cmd.c_str());
		if (res != 0)
			logger.write_Log(eLoggingOpt::ALL_PRINT, __FUNCTION__, "실행파일을 실행하지 못했습니다. : " + to_string(res) + "-" + cmd);
	}

	virtual void Flush() {
		string cmd = curPath;
		cmd.append(mExecuteName + " F");
		int res = system(cmd.c_str());
		if (res != 0)
			logger.write_Log(eLoggingOpt::ALL_PRINT, __FUNCTION__, "실행파일을 실행하지 못했습니다. : " + to_string(res) + "-" + cmd);

	}

private:
	char curPath[256];

	string mReadFileName = "result.txt";
	string mExecuteName = "\\ssd.exe";
	Logger& logger = Logger::GetInstance();
};

class createProductFactory {
public:
	createProductFactory(string productType) {
		createProduct(productType);
	}

	void createProduct(string productType) {
		if (productType == "SSD")
		{
			product = new SSDProduct;
		}
	}

	IProduct* getProduct() {
		return product;
	}


private:
	IProduct* product = nullptr;
};
