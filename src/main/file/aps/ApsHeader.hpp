#pragma once

#include <vector>

namespace mpc::file::aps
{
class ApsHeader
{
    
public:
    bool valid = false;
    int soundCount = 0;
    std::vector<char> saveBytes;
    
public:
    bool isValid();
    int getSoundAmount();
    std::vector<char> getBytes();
    
    ApsHeader(const std::vector<char>& loadBytes);
    ApsHeader(int soundCount);
};
}
