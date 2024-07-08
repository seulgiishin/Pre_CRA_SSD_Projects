#include "ICommand.h"
#include "CommandParser.cpp"

class HelpCommand : public ICommand {
public:
	// ICommand��(��) ���� ��ӵ�
	string getCommand() override
	{
		return commandName;
	}

	void execute(CommandParser& cp, IProduct* product) override
	{
		logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "Print Help Text.");
		for (string help : helpText) {
			cout << help << endl;
		}
	}

private:
	static constexpr char commandName[] = "HELP";
	const vector<string> helpText = {
	"�ֿ� ��ɾ�",
	"1. write (��) write LBA data : LBA �� data�� ���",
	"2. read (��) read LBA : LBA �� ����)",
	"3. exit (��) exit : shell �� ����)",
	"4. help (��) help : �ֿ� ��ɾ��� ����� ���)",
	"5. fullwrite (��) fullwrite data : ��� LBA(0~99)���� data �� write ����)",
	"6. fullread (��) fullread : ��� LBA(0~99) ���� read ����)",
	"7. erase (��) erase LBA size : Ư�� LBA���� Ư�� size���� ���� ����",
	"8. erase_range (��) erase_range LBA LBA : Ư�� LBA���� Ư��LBA �������� ���� ����"
	};
};