#include "ICommand.h"
#include "CommandParser.cpp"

class TestApp1Command : public ICommand {
public:
	// ICommand을(를) 통해 상속됨
	string getCommand() override
	{
		return commandName;
	}

	void execute(CommandParser& cp, IProduct* product) override
	{
		/*
		//bool ret false;
		for (int lba = 0; lba < 100; lba++) {
			product->Write(lba, TEST_PATTERN_TESTAPP1);
		}

		vector <string> results;
		for (int lba = 0; lba < MAX_LBA; lba++) {
			results.push_back(product->Read(lba));
			if (isRunningTestScenario == false)
				cout << results[lba] << endl;
		}
		cp.setResult(results);

		if (results.size() != MAX_LBA) {
			throw exception("TestApp1 Error!");
		}

		for (int lba = 0; lba < MAX_LBA; lba++) {
			if (results[lba] != TEST_PATTERN_TESTAPP1)
				throw exception("TestApp1 Error!");
		}
		*/
	}

private:
	static constexpr char commandName[] = "TESTAPP1";
};