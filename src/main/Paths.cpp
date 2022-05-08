#include "Paths.hpp"

#include <sys/Home.hpp>
#include <file/FileUtil.hpp>

#include <thirdp/PlatformFolders/platform_folders.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

using namespace mpc;
using namespace moduru::file;
using namespace std;

string Paths::sep = FileUtil::getSeparator();

string Paths::appDocumentsPath()
{
#if TARGET_OS_IOS
    static auto path = sago::getDocumentsFolder() + sep;
#else
    static auto path = sago::getDocumentsFolder() + sep + "VMPC2000XL" + sep;
#endif
    return path;
}

string Paths::appConfigHome()
{
    static auto path = sago::getConfigHome() + sep + "VMPC2000XL" + sep;
    return path;
}

string Paths::appConfigPath()
{
    static auto path = sago::getConfig() + sep + "VMPC2000XL" + sep;
    return path;
}

string Paths::configPath()
{
    static auto path = appConfigHome() + "config" + sep;
    return path;
}

string Paths::storesPath()
{
  static auto storesPath = appDocumentsPath() + "Volumes" + sep;
  return storesPath;
}

string Paths::defaultLocalVolumePath()
{
    static auto storesPath = appDocumentsPath() + "Volumes" + sep + "MPC2000XL";
    return storesPath;
}

string Paths::logFilePath()
{
  static auto logFilePath = appDocumentsPath() + "vmpc.log";
  return logFilePath;
}

string Paths::recordingsPath()
{
    static auto recPath = appDocumentsPath() + "Recordings" + sep;
    return recPath;
}

string Paths::demoDataSrcPath()
{
    static auto demoDataPath = sago::getData() + sep + "VMPC2000XL" + sep + "DemoData" + sep;
    return demoDataPath;
}

string Paths::demoDataDestPath()
{
    static auto demoDataPath = defaultLocalVolumePath() + sep + "Demos" + sep;
    return demoDataPath;
}
