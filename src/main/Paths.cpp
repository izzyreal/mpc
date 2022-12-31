#include "Paths.hpp"

#include <file/FileUtil.hpp>

#include <thirdp/PlatformFolders/platform_folders.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

using namespace mpc;
using namespace moduru::file;

std::string Paths::sep()
{
    static auto res = FileUtil::getSeparator();
    return res;
}

std::string Paths::appDocumentsPath()
{
#if TARGET_OS_IOS
    static auto path = sago::getDocumentsFolder() + sep();
#else
    static auto path = sago::getDocumentsFolder() + sep() + "VMPC2000XL" + sep();
#endif
    return path;
}

std::string Paths::appConfigHome()
{
    static auto path = sago::getConfigHome() + sep() + "VMPC2000XL" + sep();
    return path;
}

std::string Paths::configPath()
{
    static auto path = appConfigHome() + "config" + sep();
    return path;
}

std::string Paths::storesPath()
{
  static auto storesPath = appDocumentsPath() + "Volumes" + sep();
  return storesPath;
}

std::string Paths::defaultLocalVolumePath()
{
    static auto storesPath = appDocumentsPath() + "Volumes" + sep() + "MPC2000XL";
    return storesPath;
}

std::string Paths::logFilePath()
{
  static auto logFilePath = appDocumentsPath() + "vmpc.log";
  return logFilePath;
}

std::string Paths::recordingsPath()
{
    static auto recPath = appDocumentsPath() + "Recordings" + sep();
    return recPath;
}

std::string Paths::midiControlPresetsPath()
{
    static auto path = appDocumentsPath() + "MidiControlPresets" + sep();
    return path;
}

std::string Paths::autoSavePath()
{
    static auto path = appDocumentsPath() + "AutoSave" + sep();
    return path;
}

std::string Paths::demoDataSrcPath()
{
    static auto demoDataPath = sago::getData() + sep() + "VMPC2000XL" + sep() + "DemoData" + sep();
    return demoDataPath;
}

std::string Paths::demoDataDestPath()
{
    static auto demoDataPath = defaultLocalVolumePath() + sep() + "Demos" + sep();
    return demoDataPath;
}
