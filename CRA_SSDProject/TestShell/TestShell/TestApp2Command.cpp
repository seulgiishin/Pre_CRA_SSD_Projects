#include "ICommand.h"
#include "CommandParser.cpp"

class TestApp2Command : public ICommand {
public:
	// ICommand을(를) 통해 상속됨
	string getCommand() override
	{
		return commandName;
	}

	void execute(CommandParser& cp, IProduct* product) override
	{
		/*
		for (int cnt = 0; cnt < 30; cnt++) {
			for (int lbaAddress = 0; lbaAddress <= 5; lbaAddress++) {
				product->Write(lbaAddress, "0xAAAABBBB");
			}
		}

		for (int lbaAddress = 0; lbaAddress <= 5; lbaAddress++) {
			product->Write(lbaAddress, "0x12345678");
		}

		for (int lbaAddress = 0; lbaAddress <= 5; lbaAddress++) {
			string result = product->Read(lbaAddress);
			if (isRunningTestScenario == false)
				cout << result << endl;
		}
		*/
	}

private:
	static constexpr char commandName[] = "TESTAPP2";
};