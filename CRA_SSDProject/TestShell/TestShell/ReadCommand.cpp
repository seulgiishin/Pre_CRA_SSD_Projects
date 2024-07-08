#include "ICommand.h"
#include "CommandParser.cpp"

class ReadCommand : public ICommand {
public:
	// ICommand을(를) 통해 상속됨
	string getCommand() override
	{
		return commandName;
	}

	void execute(CommandParser& cp, IProduct* product) override
	{
		vector<string> results;
		checkInvalid_LBA(cp.lba);
		string result = product->Read(cp.lba);
		if (isRunningTestScenario == false)
			logger.write_Log(eLoggingOpt::ALL_PRINT, __FUNCTION__, "[LBA] Data : [" + to_string(cp.lba) + "] " + result);

		results.push_back(result);
		cp.setResult(results);
	}

private:
	static constexpr char commandName[] = "READ";
};