#include "ICommand.h"
#include "CommandParser.cpp"

class HelpCommand : public ICommand {
public:
	// ICommand을(를) 통해 상속됨
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
	"주요 명령어",
	"1. write (예) write LBA data : LBA 에 data를 기록",
	"2. read (예) read LBA : LBA 를 읽음)",
	"3. exit (예) exit : shell 을 종료)",
	"4. help (예) help : 주요 명령어의 사용방법 출력)",
	"5. fullwrite (예) fullwrite data : 모든 LBA(0~99)까지 data 를 write 수행)",
	"6. fullread (예) fullread : 모든 LBA(0~99) 까지 read 수행)",
	"7. erase (예) erase LBA size : 특정 LBA부터 특정 size까지 내용 삭제",
	"8. erase_range (예) erase_range LBA LBA : 특정 LBA부터 특정LBA 직전까지 내용 삭제"
	};
};