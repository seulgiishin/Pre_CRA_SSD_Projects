#include "ICommand.h"

class WriteCommand : public ICommand {
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
		logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "Write Data (LBA) : " + cp.data + "(" + to_string(cp.lba) + ")");
	}

private:
	static constexpr char commandName[] = "WRITE";
};