#pragma once
#include "ICommand.h"
#include "CommandParser.cpp"

class EraseCommand : public ICommand {
public:
	// ICommand을(를) 통해 상속됨
	string getCommand() override
	{
		return commandName;
	}

	void execute(CommandParser& cp, IProduct* product) override
	{
		checkInvalid_LBA(cp.lba);

		int tmp = cp.size;
		for (int i = 0; i < (cp.size / 10 + 1); i++) {
			if (tmp >= 10) {
				product->Erase(cp.lba + i * 10, 10);
				tmp -= 10;
				logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "Erase  : " + to_string(cp.lba + i * 10) + " ~ " + to_string(cp.lba + i * 10 + 9));
			}
			else if (tmp != 0) {
				product->Erase(cp.lba + i * 10, tmp);
				logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "Erase  : " + to_string(cp.lba + i * 10) + " ~ " + to_string(cp.lba + i * 10 + tmp - 1));
			}
		}
	}

private:
	static constexpr char commandName[] = "ERASE";
};