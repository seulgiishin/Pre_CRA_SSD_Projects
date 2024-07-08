#include <iostream>
#include <vector>
#include <memory>
#include "ICommand.h"
#include "ssdexcept.h"

class CommandInvoker {
private:
    std::vector<std::unique_ptr<ICommand>> commands;
    Storage* storage = nullptr;

public:
    CommandInvoker(Storage* storage) : storage(storage) {}
    void addCommand(unique_ptr<ICommand> command) {
        commands.push_back(std::move(command));
    }

    int executeCommands(int argc, char* argv[]) {
        if (argc <= 1)
            throw ssd_exception("[Argument Validation] Empty Command Code");

        for (const auto& command : commands) {
            if (argc >= 2 && argv[1] == command->getCommandCode())
            {
                return command->execute(argc, argv, storage);
            }
        }
        throw ssd_exception(string("[Argument Validation] Invalid Command Code: ") + argv[1]);
    }

    void printHelp() {
        std::cout << "======================= Help =======================" << std::endl;
        for (const auto& command : commands) {
                std::cout << command->getHelpMessage() << std::endl;
        }
        std::cout << "====================================================" << std::endl;
    }
};
