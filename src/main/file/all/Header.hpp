#pragma once

#include <vector>

namespace mpc::file::all {
class Header
{
public:
    std::vector<char> loadBytes;
    std::vector<char> saveBytes;
    
public:
    std::vector<char> getHeaderArray();
    bool verifyFileID();
    
public:
    std::vector<char> getBytes();
    
    Header(const std::vector<char>& b);
    Header();
};
}
