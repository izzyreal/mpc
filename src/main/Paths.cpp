#include "Paths.hpp"

#include <platform_folders.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

using namespace mpc;

fs::path Paths::appDocumentsPath()
{
#if TARGET_OS_IOS
    static auto path = fs::path(sago::getDocumentsFolder());
#else
    static auto path = fs::path(sago::getDocumentsFolder()) / "VMPC2000XL";
#endif
    return path;
}

fs::path Paths::appConfigHome()
{
    static auto path = fs::path(sago::getConfigHome()) / "VMPC2000XL";
    return path;
}

fs::path Paths::configPath()
{
    static auto path = appConfigHome() / "config";
    return path;
}

fs::path Paths::storesPath()
{
  static auto storesPath = appDocumentsPath() / "Volumes";
  return storesPath;
}

fs::path Paths::defaultLocalVolumePath()
{
    static auto storesPath = appDocumentsPath() / "Volumes" / "MPC2000XL";
    return storesPath;
}

fs::path Paths::logFilePath()
{
  static auto logFilePath = appDocumentsPath() / "vmpc.log";
  return logFilePath;
}

fs::path Paths::recordingsPath()
{
    static auto recPath = appDocumentsPath() / "Recordings";
    return recPath;
}

fs::path Paths::midiControlPresetsPath()
{
    static auto path = appDocumentsPath() / "MidiControlPresets";
    return path;
}

fs::path Paths::autoSavePath()
{
    static auto path = appDocumentsPath() / "AutoSave";
    return path;
}

fs::path Paths::demoDataPath()
{
    static auto demoDataPath = defaultLocalVolumePath() / "Demos";
    return demoDataPath;
}

fs::path Paths::tempPath()
{
    static auto tempPath = appDocumentsPath() / "Temp";
    return tempPath;
}