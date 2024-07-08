#include <iostream>
#include <memory>
#include <stdexcept>
#include "Invoker.cpp"
#include "Write.cpp"
#include "Read.cpp"
#include "Erase.cpp"
#include "Flush.cpp"

int main(int argc, char* argv[])
{
    Device* device = new StorageDevice();
    Storage* ssd = device->setDevice(TYPE_SSD);
    CommandInvoker invoker(ssd);

    invoker.addCommand(std::move(std::make_unique<WriteCommand>()));
    invoker.addCommand(std::move(std::make_unique<ReadCommand>()));
    invoker.addCommand(std::move(std::make_unique<EraseCommand>()));
    invoker.addCommand(std::move(std::make_unique<FlushCommand>()));
 
    if (argc < 2) {
        invoker.printHelp();
        return 0;
    }

    try {
        return invoker.executeCommands(argc, argv);
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        return -EPERM;
    }
    return 0;
}