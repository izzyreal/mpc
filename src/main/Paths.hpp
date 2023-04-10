#pragma once

#include "mpc_fs.hpp"

namespace mpc {
class Paths
{
private:
    static fs::path appConfigHome();

public:
    static fs::path appDocumentsPath();

    static fs::path storesPath();
    static fs::path defaultLocalVolumePath();

    static fs::path logFilePath();
    static fs::path recordingsPath();
    static fs::path midiControlPresetsPath();
    static fs::path autoSavePath();

    static fs::path configPath();
    static fs::path demoDataSrcPath();
    static fs::path demoDataDestPath();

};
}
