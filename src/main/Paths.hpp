#pragma once

#include <string>

namespace mpc {
class Paths
{
private:
    static std::string home();
    static std::string sep;
    
public:
    static std::string resPath();
    static std::string tempPath();
    static std::string storesPath();
    static std::string logFilePath();
    static std::string recordingsPath();

};
}
