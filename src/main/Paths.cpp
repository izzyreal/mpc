#include "Paths.hpp"

#include <sys/Home.hpp>
#include <file/FileUtil.hpp>

#include <thirdp/PlatformFolders/platform_folders.h>

using namespace mpc;
using namespace moduru::file;
using namespace std;

string Paths::sep = FileUtil::getSeparator();

string Paths::appDocumentsPath()
{
    static auto path = sago::getDocumentsFolder() + sep + "VMPC2000XL" + sep;
    return path;
}

string Paths::appDataPath()
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
    static auto path = appConfigPath() + "config" + sep;
    return path;
}

string Paths::storesPath()
{
	static auto storesPath = appDocumentsPath() + "Volumes" + sep;
	return storesPath;
}

string Paths::defaultStorePath()
{
    static auto storesPath = appDocumentsPath() + "Volumes" + sep + "MPC2000XL" + sep;
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
    static auto demoDataPath = appDataPath() + "DemoData" + sep;
    return demoDataPath;
}

string Paths::demoDataDestPath()
{
    static auto demoDataPath = defaultStorePath() + "Demos" + sep;
    return demoDataPath;
}
