#include <string>
#include <regex>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace util
{
std::string IntToHexUppercaseString(int data)
{
    std::stringstream dataToHex;
    dataToHex << "0x" << std::hex << std::setw(8) << std::setfill('0') << std::uppercase << data;
    return dataToHex.str();
}

std:: string GetClassMethodName(const std::string &funcSig) {
    std::regex regex(R"((__cdecl|__thiscall|__stdcall|__fastcall)?[\s\S]* (\w+::\w+)\(.*\))");
    std::smatch match;

    if (std::regex_search(funcSig, match, regex) && match.size() > 2) {
        return match[2].str(); // 두 번째 그룹을 반환
    }
    return "";
}
}
