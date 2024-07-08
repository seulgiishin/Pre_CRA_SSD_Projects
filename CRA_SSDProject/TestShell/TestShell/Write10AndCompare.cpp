#include "ICommand.h"

class Write10AndCompare : public ICommand {
public:
	// ICommand을(를) 통해 상속됨
	string getCommand() override
	{
		return commandName;
	}

	void execute(CommandParser& cp, IProduct* product) override
	{
		checkInvalid_LBA(cp.lba);
		checkInvalid_Value(cp.data);
		product->Write(cp.lba, cp.data);
		string expected = product->Read(cp.lba);
		logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "Write Data (LBA) : " + cp.data + "(" + to_string(cp.lba) + ")");
		for (int cnt = 0; cnt < 9; cnt++) {
			product->Write(cp.lba, cp.data);
			string result = product->Read(cp.lba);
			logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "Write Data (LBA) : " + cp.data + "(" + to_string(cp.lba) + ")");
			if (expected != result)
				throw exception("Write10AndCompare Scenario Error!");
		}
	}

private:
	static constexpr char commandName[] = "WRITE10ANDCOMPARE";
};