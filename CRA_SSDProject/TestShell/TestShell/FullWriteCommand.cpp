#include "ICommand.h"
#include "CommandParser.cpp"

class FullWriteCommand : public ICommand {
public:
	// ICommand��(��) ���� ��ӵ�
	string getCommand() override
	{
		return commandName;
	}

	void execute(CommandParser& cp, IProduct* product) override
	{
		logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "Full Write SSD Data");
		for (int lba = 0; lba < 100; lba++) {
			product->Write(lba, cp.data);
			logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "[LBA] Data  : [" + to_string(lba) + "] " + cp.data);
		}
	}

private:
	static constexpr char commandName[] = "FULLWRITE";
};