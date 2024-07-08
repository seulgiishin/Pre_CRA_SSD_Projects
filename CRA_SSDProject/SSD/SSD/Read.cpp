#include "ICommand.h"

class ReadCommand : public ICommand {
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
			storage->read(cmd.address);
		return ret;
	}

private:
	static constexpr char COMMAND_CODE[] = "R";
	static constexpr char HELP[] = "[Read] ssd R [address: 0~99]";
};