#pragma once

#include <string>

namespace mpc {
class Paths
{
private:
//    static std::string override
    static std::string sep();

public:
    static std::string appConfigHome();
    static std::string appDocumentsPath();

    static std::string storesPath();
    static std::string defaultLocalVolumePath();

    static std::string logFilePath();
    static std::string recordingsPath();
    static std::string midiControlPresetsPath();

    static std::string configPath();
    static std::string demoDataSrcPath();
    static std::string demoDataDestPath();

};
}
