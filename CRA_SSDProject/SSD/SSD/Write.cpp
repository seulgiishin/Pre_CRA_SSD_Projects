#include "ICommand.h"

class WriteCommand : public ICommand {
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
			storage->write(cmd.address, cmd.data);
		return ret;
	}

private:
	static constexpr char COMMAND_CODE[] = "W";
	static constexpr char HELP[] = "[Write] ssd W [address: 0~99] [data: 0x#######]";
};