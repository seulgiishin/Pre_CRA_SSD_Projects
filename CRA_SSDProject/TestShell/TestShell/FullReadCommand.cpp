#include "ICommand.h"
#include "CommandParser.cpp"

class FullReadCommand : public ICommand {
public:
	// ICommand을(를) 통해 상속됨
	string getCommand() override
	{
		return commandName;
	}

	void execute(CommandParser& cp, IProduct* product) override
	{
		logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "Full Read SSD Data");
		vector<string> results;
		for (int lba = 0; lba < MAX_LBA; lba++) {
			results.push_back(product->Read(lba));
			if (isRunningTestScenario == false)
				logger.write_Log(eLoggingOpt::ALL_PRINT, __FUNCTION__, "[LBA] Data  : [" + to_string(lba) + "] " + results[lba]);
		}
		//return results;
		cp.setResult(results);
	}

private:
	static constexpr char commandName[] = "FULLREAD";
};