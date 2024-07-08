#include "ICommand.h"
#include "CommandParser.cpp"

class FlushCommand : public ICommand {
public:
	// ICommand��(��) ���� ��ӵ�
	string getCommand() override
	{
		return commandName;
	}

	void execute(CommandParser& cp, IProduct* product) override
	{
		checkInvalid_LBA(cp.lba);
		product->Flush();
		logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "Complete Flush");
	}

private:
	static constexpr char commandName[] = "FLUSH";
};