#include "ICommand.h"

class FlushCommand : public ICommand {
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
			storage->flush();
		return ret;
	}

private:
	static constexpr char COMMAND_CODE[] = "F";
	static constexpr char HELP[] = "[Flush] ssd F";
};