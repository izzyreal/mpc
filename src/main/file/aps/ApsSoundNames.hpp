#pragma once

#include <vector>
#include <string>

namespace mpc::sampler {
class Sampler;
}

namespace mpc::file::aps {
class ApsSoundNames
{
public:
    ApsSoundNames(const std::vector<char>& loadBytes);
    ApsSoundNames(mpc::sampler::Sampler* sampler);
    
    static const int NAME_STRING_LENGTH = 16;
    
    std::vector<std::string> names;
    std::vector<char> saveBytes;
    
    std::vector<std::string> get();
    std::vector<char> getBytes();
    
};
}
