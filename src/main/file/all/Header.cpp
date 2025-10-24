#include "file/all/Header.hpp"

#include <Util.hpp>

#include <string>

using namespace mpc::file::all;

Header::Header(const std::vector<char> &_loadBytes)
    : loadBytes(_loadBytes)
{
}

Header::Header()
{
    const std::string s = "MPC2KXL ALL 1.00";
    saveBytes = std::vector<char>(16);
    for (int i = 0; i < 16; i++)
    {
        saveBytes[i] = s[i];
    }
}

std::vector<char> &Header::getHeaderArray()
{
    return loadBytes;
}

bool Header::verifyFileID()
{
    auto verifyFileID = false;
    auto checkFileID = Util::vecCopyOfRange(loadBytes, 0, 16);
    std::string fileIDString;
    for (char c : checkFileID)
    {
        if (c == 0x00)
        {
            break;
        }
        fileIDString.push_back(c);
    }
    std::string shouldBe = "MPC2KXL ALL 1.00";
    if (fileIDString == shouldBe)
    {
        verifyFileID = true;
    }
    return verifyFileID;
}

std::vector<char> &Header::getBytes()
{
    return saveBytes;
}
