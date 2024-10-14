#pragma once

#include <vector>
#include <cstdint>

namespace mpc::file::aps
{
class ApsHeader
{
private:
    const std::vector<char> APS_HEADER_MAGIC{ 0x0A, 0x05 };
    
public:
    bool valid = false;
    uint16_t soundCount = 0;
    std::vector<char> saveBytes;
    
public:
    bool isValid() const;
    int getSoundAmount() const;
    std::vector<char> getBytes() const;
    
    explicit ApsHeader(const std::vector<char>& loadBytes);
    explicit ApsHeader(uint16_t soundCount);
};
}
