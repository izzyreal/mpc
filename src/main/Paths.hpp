#pragma once

#include <string>

namespace mpc {
class Paths
{
private:
    static std::string sep;
        
    static std::string appDocumentsPath();
    static std::string appDataPath();
    static std::string appConfigPath();

public:
    static std::string storesPath();
    
    static std::string logFilePath();
    static std::string recordingsPath();
    
    static std::string fontsPath();
    static std::string imgPath();
    static std::string screensPath();
    static std::string bgPath();
    static std::string audioPath();
    static std::string configPath();
    
};
}
