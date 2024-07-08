#include "ICommand.h"
#include "CommandParser.cpp"

class FullRead10AndCompare : public ICommand {
public:
	// ICommand을(를) 통해 상속됨
	string getCommand() override
	{
		return commandName;
	}

	void execute(CommandParser& cp, IProduct* product) override
	{
		logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "FullRead10AndCompare");
		vector<string> expected;
		for (int lba = 0; lba < MAX_LBA; lba++) {
			expected.push_back(product->Read(lba));
			if (isRunningTestScenario == false)
				cout << expected[lba] << endl;
			logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "[LBA] Data  : [" + to_string(lba) + "] " + expected[lba]);
		}
		for (int cnt = 0; cnt < 9; cnt++) {
			vector<string> results;
			for (int lba = 0; lba < MAX_LBA; lba++) {
				results.push_back(product->Read(lba));
				if (isRunningTestScenario == false)
					cout << results[lba] << endl;
				logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "[LBA] Data  : [" + to_string(lba) + "] " + results[lba]);
			}
			for (int lba = 0; lba < MAX_LBA; lba++) {
				if(results[lba]  != expected[lba])
					throw exception("FullRead10AndCompare Scenario Error!");
			}
		}
		cp.setResult(expected);
	}

private:
	static constexpr char commandName[] = "FULLREAD10ANDCOMPARE";
};