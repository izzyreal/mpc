#pragma once

#include <string>

namespace mpc {
class Paths
{
private:
    static std::string sep;

public:
    static std::string appDocumentsPath();
    static std::string appDataPath();
    static std::string appConfigPath();

    static std::string storesPath();
    static std::string defaultStorePath();

    static std::string logFilePath();
    static std::string recordingsPath();
    
    static std::string imgPath();

//    static std::string fontsPath();
//    static std::string screensPath();
//    static std::string bgPath();
//    static std::string audioPath();
    static std::string configPath();
    
};
}
