#pragma once

#include <vector>
#include <string>

namespace mpc {
    class MpcResourceUtil {

    public:
        static std::vector<char> get_resource_data(const std::string &path);
    };
}
