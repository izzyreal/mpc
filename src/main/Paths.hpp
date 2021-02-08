#pragma once

#include <string>

namespace mpc {
class Paths
{
public:
    static std::string home();
    static std::string resPath();
    static std::string tempPath();
    static std::string storesPath();
    static std::string logFilePath();
};
}
