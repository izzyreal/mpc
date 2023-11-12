#pragma once

#include "mpc_fs.hpp"

namespace mpc {
class Paths
{
protected:
    virtual fs::path appConfigHome();

public:
    virtual fs::path appDocumentsPath();

    fs::path storesPath();
    fs::path defaultLocalVolumePath();

    fs::path logFilePath();
    fs::path recordingsPath();
    fs::path midiControlPresetsPath();
    fs::path autoSavePath();

    fs::path configPath();
    fs::path demoDataPath();
    fs::path tempPath();

};
}
