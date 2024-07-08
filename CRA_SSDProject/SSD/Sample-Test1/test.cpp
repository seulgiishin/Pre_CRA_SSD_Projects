#include <stdexcept>
#include <memory>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <windows.h>

#include "../SSD/ICommand.h"
#include "../SSD/Invoker.cpp"
#include "../SSD/Write.cpp"
#include "../SSD/Read.cpp"
#include "../SSD/Erase.cpp"
#include "../SSD/Flush.cpp"
#include "../SSD/util.cpp"

class SSDFIxture : public testing::Test {
public:
    const string OUTPUT = "result.txt";
    const string NAND = "nand.txt";
    const string CMDFILE = "cmdlist.txt";

    Device* device = new StorageDevice();
    Storage* ssd = device->setDevice(TYPE_SSD);
    CommandInvoker invoker{ ssd };

    void SetUp() override {
        invoker.addCommand(std::move(std::make_unique<WriteCommand>()));
        invoker.addCommand(std::move(std::make_unique<ReadCommand>()));
        invoker.addCommand(std::move(std::make_unique<EraseCommand>()));
        invoker.addCommand(std::move(std::make_unique<FlushCommand>()));
    }

    void TearDown() override {
        DeleteFile(wstring(NAND.begin(), NAND.end()).c_str());
        DeleteFile(wstring(OUTPUT.begin(), OUTPUT.end()).c_str());
        DeleteFile(wstring(CMDFILE.begin(), CMDFILE.end()).c_str());
    };

    int readResult()
    {
        ifstream inFile(OUTPUT);

        if (!inFile.is_open()) {
            throw ssd_exception("Cannot Open File");
        }

        string line;
        getline(inFile, line);
        inFile.close();

        return std::stoul(line, nullptr, 16);
    }

    int makeReadCmd(char* address) {
        char* argv[3];

        argv[0] = "ssd.exe";
        argv[1] = "R";
        argv[2] = address;
        return invoker.executeCommands(3, argv);
    }

    int makeWriteCmd(char* address, char* value) {
        char* argv[4];

        argv[0] = "ssd.exe";
        argv[1] = "W";
        argv[2] = address;
        argv[3] = value;
        return invoker.executeCommands(4, argv);
    }

    int makeEraseCmd(char* address, char* size) {
        char* argv[4];

        argv[0] = "ssd.exe";
        argv[1] = "E";
        argv[2] = address;
        argv[3] = size;
        return invoker.executeCommands(4, argv);
    }

    int  makeFlushCmd() {
        char* argv[2];

        argv[0] = "ssd.exe";
        argv[1] = "F";
        return invoker.executeCommands(2, argv);
    }

    vector<string> getSsdData() {
        vector<string> data;
        ifstream inFile(NAND);

        if (!inFile.is_open()) {
            return data;
        }

        string line;
        while (getline(inFile, line)) {
            data.push_back(line);
        }
        inFile.close();

        return data;
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

TEST_F(SSDFIxture, SsdWrite0) {
    int address = 0;
    int data = 0xdeadbeef;
    ssd->write(address, data);
}

TEST_F(SSDFIxture, SsdWrite99) {
    int address = 0;
    int data = 0xdeadbeef;
    ssd->write(address, data);
}

TEST_F(SSDFIxture, SsdWrite100) {
    int address = 100;
    int data = 0xdeadbeef;
    EXPECT_THROW({
         ssd->write(address, data);
        }, ssd_exception);
}

TEST_F(SSDFIxture, SsdRead0) {
    int address = 0;
    int expected = 0x00000000;
    ssd->read(address);
    
    EXPECT_THAT(readResult(), testing::Eq(expected));
}

TEST_F(SSDFIxture, SsdRead99) {
    int address = 99;
    int expected = 0x00000000;
    ssd->read(address);
    EXPECT_THAT(readResult(), testing::Eq(expected));
}

TEST_F(SSDFIxture, SsdRead100) {
    int address = 100;
    int expected = 0x00000000;
    EXPECT_THROW({
         ssd->read(address);
        }, ssd_exception);
}

TEST_F(SSDFIxture, SsdBrokenFile) {
    ofstream testFile(NAND);
    for (int i = 0; i < 1; i++) {
        testFile << "0" << endl;
    }
    testFile.close();

    EXPECT_THROW({
         ssd->read(0);
        }, ssd_exception);
}

TEST_F(SSDFIxture, CommandInvokerEmptyCommand) {
    int argc = 1;
    char* argv[] = { "ssd.exe" };

    EXPECT_THROW({
        invoker.executeCommands(argc, argv);
        }, ssd_exception);
}

TEST_F(SSDFIxture, CommandInvokerEmptyArgument) {
    int argc = 1;
    char* argv[] = { "ssd.exe", "W"};

    EXPECT_THROW({
        invoker.executeCommands(argc, argv);
        }, ssd_exception);
}

TEST_F(SSDFIxture, CommandInvokerLessArgument) {
    int argc = 1;
    char* argv[] = { "ssd.exe", "W", "0"};

    EXPECT_THROW({
        invoker.executeCommands(argc, argv);
        }, ssd_exception);
}

TEST_F(SSDFIxture, CommandInvokerWrite0) {
    int ret = makeWriteCmd("0", "0xdeadbeef");

    EXPECT_THAT(ret, testing::Eq(ICommand::COMMAND_VALIDATION_SUCCESS));
}

TEST_F(SSDFIxture, CommandInvokerRead0) {
    int expectedData = 0x00000000;

    makeReadCmd("0");
    EXPECT_THAT(readResult(), testing::Eq(expectedData));
}

TEST_F(SSDFIxture, CommandInvokerWrite100) {
    EXPECT_THROW({
         makeWriteCmd("100", "0xdeadbeef");
        }, ssd_exception);
}

TEST_F(SSDFIxture, CommandInvokerRead100) {

    EXPECT_THROW({
          makeReadCmd("100");
        }, ssd_exception);
}

TEST_F(SSDFIxture, CommandInvokerWriteRead0Verify) {
    makeWriteCmd("0", "0xdeadbeef");
    makeReadCmd("0");

    int expectedData = 0xdeadbeef;
    EXPECT_THAT(readResult(), testing::Eq(expectedData));
}

TEST_F(SSDFIxture, CommandInvokerWriteRead50Verify) {
    makeWriteCmd("50", "0xdeadbeef");
    makeReadCmd("50");

    int expectedData = 0xdeadbeef;
    EXPECT_THAT(readResult(), testing::Eq(expectedData));
}


TEST_F(SSDFIxture, CommandInvokerWriteInvalidData) {
    EXPECT_THROW({
         makeWriteCmd("0", "0xzzzzzzzz");
        }, ssd_exception);
}

TEST_F(SSDFIxture, CommandInvokerWriteInvalidDataLong) {
    EXPECT_THROW({
         makeWriteCmd("0", "0xdeadbeefffff");
        }, ssd_exception);
}

TEST_F(SSDFIxture, CommandInvokerReadInvalidAddressFormat0x0) {
    int argc = 3;
    char* argv[] = { "ssd.exe", "R", "0x0"};
    EXPECT_THROW({
         invoker.executeCommands(argc, argv);
        }, ssd_exception);
}

TEST_F(SSDFIxture, CommandInvokerWriteReadEraseReadVerify) {
    makeWriteCmd("0", "0xdeadbeef");
    makeReadCmd("0");

    int expectedData = 0xdeadbeef;
    EXPECT_THAT(readResult(), testing::Eq(expectedData));

    makeEraseCmd("0", "1");
    makeReadCmd("0");

    expectedData = 0x00000000;
    EXPECT_THAT(readResult(), testing::Eq(expectedData));
}

TEST_F(SSDFIxture, CommandInvokerWriteAndErase) {
    makeWriteCmd("10", "0xdeadbeef");
    makeWriteCmd("10", "0x00000001");
    makeReadCmd("10");

    int expectedData = 0x00000001;
    EXPECT_THAT(readResult(), testing::Eq(expectedData));
 
    makeEraseCmd("9", "3");
    makeReadCmd("10");
    expectedData = 0x00000000;
    EXPECT_THAT(readResult(), testing::Eq(expectedData));
}

TEST_F(SSDFIxture, CommandInvokerWriteFlushErase) {
    makeWriteCmd("3", "0x00000003");
    makeFlushCmd();

    makeEraseCmd("0", "10");
    makeReadCmd("3");

    int expectedData = 0x00000000;
    EXPECT_THAT(readResult(), testing::Eq(expectedData));
}

TEST_F(SSDFIxture, CommandInvokerFlush10) {
    makeWriteCmd("10", "0x00000010");
    makeWriteCmd("20", "0x00000020");
    makeWriteCmd("30", "0x00000030");
    makeWriteCmd("40", "0x00000040");
    makeWriteCmd("50", "0x00000050");
    makeWriteCmd("60", "0x00000060");
    makeWriteCmd("70", "0x00000070");
    makeWriteCmd("80", "0x00000080");
    makeWriteCmd("90", "0x00000090");

    vector<string> data = getSsdData();
    EXPECT_THAT(data.empty(), TRUE);

    makeWriteCmd("1", "0x00000001");
    data = getSsdData();
    EXPECT_THAT(data[1], testing::Eq("0x00000001"));
}

TEST_F(SSDFIxture, CommandInvokerFastWriteCase1) {
    makeEraseCmd("10", "5");
    makeEraseCmd("15", "5");

    vector<CommandSet> cmdset = getCommandList();
    EXPECT_THAT(cmdset.size(), testing::Eq(1));
    EXPECT_THAT(cmdset[0].cmdOpcode, testing::Eq(SSD::COMMAND_ERASE));
    EXPECT_THAT(cmdset[0].address, testing::Eq(10));
    EXPECT_THAT(cmdset[0].size, testing::Eq(10));
}

TEST_F(SSDFIxture, CommandInvokerFastWriteCase2) {
    makeWriteCmd("3", "0xdeadbeef");
    makeEraseCmd("0", "5");

    vector<CommandSet> cmdset = getCommandList();
    EXPECT_THAT(cmdset.size(), testing::Eq(1));
    EXPECT_THAT(cmdset[0].cmdOpcode, testing::Eq(SSD::COMMAND_ERASE));
    EXPECT_THAT(cmdset[0].address, testing::Eq(0));
    EXPECT_THAT(cmdset[0].size, testing::Eq(5));
}

TEST_F(SSDFIxture, CommandInvokerFastRead) {
    makeWriteCmd("3", "0xdeadbeef");
    makeReadCmd("3");

    int expectedData = 0xdeadbeef;
    EXPECT_THAT(readResult(), testing::Eq(expectedData));

    vector<string> data = getSsdData();
    EXPECT_EQ(data.empty(), TRUE);
}