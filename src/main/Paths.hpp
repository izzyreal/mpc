#pragma once

#include <string>

namespace mpc {
class Paths
{
private:
    static std::string sep;

    static std::string appConfigHome();

public:
    static std::string appDocumentsPath();
    static std::string appConfigPath();

    static std::string storesPath();
    static std::string defaultStorePath();

    static std::string logFilePath();
    static std::string recordingsPath();
    
    static std::string configPath();
    static std::string demoDataSrcPath();
    static std::string demoDataDestPath();
    
};
}
