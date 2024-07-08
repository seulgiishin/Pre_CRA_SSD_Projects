#include <vector>

#include "ProductInterface.h"
#include "ICommand.h"
#include "Logger.h"
#include "CommandParser.cpp"

class CommandInvoker {
public:
	void setProduct(IProduct* iproduct) {
		product = iproduct;
	}

	void createProduct(string device) {
		createProductFactory cPF(device);
		setProduct(cPF.getProduct());
	}

	void addCommand(unique_ptr<ICommand> command) {
		commands.push_back(std::move(command));
	}

	void execute(CommandParser& cp) {
		for (const auto& command : commands) {
			if (command->getCommand() == cp.cmd) {
				command->execute(cp, product);
				logger.write_Log(command->getLoggingMode(), __FUNCTION__, cp.cmd + " Command execute");
				break;
			}
		}
	}

	void setRunner(bool runflag) {
		for (const auto& command : commands) {
			command->setRunnerMode(runflag);
		}
		logger.write_Log(eLoggingOpt::ONLY_FILE, __FUNCTION__, "set runflag : " + runflag);
	}

private:
	IProduct* product;
	std::vector<std::unique_ptr<ICommand>> commands;
	Logger& logger = Logger::GetInstance();
};