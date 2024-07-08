#include "ICommand.h"

class EraseCommand : public ICommand {
public:
	virtual string getCommandCode() const override {
		return COMMAND_CODE;
	}

	virtual string getHelpMessage() const override {
		return HELP;
	}

	virtual int execute(int argc, char* argv[], Storage* storage)
	{
		CommandSet cmd;
		int ret = checkParameter(argc, argv, cmd);
		if (ret == ICommand::COMMAND_VALIDATION_SUCCESS)
			storage->erase(cmd.address, cmd.size);
		return ret;
	}

private:
	static constexpr char COMMAND_CODE[] = "E";
	static constexpr char HELP[] = "[Erase] ssd E [address: 0~99] [size: 1~10]";
};