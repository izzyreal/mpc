#include "Paths.hpp"

#include <sys/Home.hpp>
#include <file/FileUtil.hpp>

#include <thirdp/PlatformFolders/platform_folders.h>

using namespace mpc;
using namespace moduru::file;
using namespace std;

string Paths::sep = FileUtil::getSeparator();

//----
string Paths::appDocumentsPath()
{
    static auto path = sago::getDocumentsFolder() + sep + "VMPC2000XL" + sep;
    return path;
}

string Paths::appDataPath()
{
    static auto path = sago::getData() + sep + "VMPC2000XL" + sep;
    return path;
}

string Paths::appConfigPath()
{
    static auto path = sago::getConfig() + sep + "VMPC2000XL" + sep;
    return path;
}
//----

//string Paths::fontsPath()
//{
//    static auto path = appDataPath()  + "fonts" + sep;
//    return path;
//}

string Paths::imgPath()
{
    static auto path = appDataPath() + "img" + sep;
    return path;
}

//string Paths::screensPath()
//{
//    static auto path = appDataPath()  + "screens" + sep;
//    return path;
//}

//string Paths::bgPath()
//{
//    static auto path = screensPath()  + "bg" + sep;
//    return path;
//}

//string Paths::audioPath()
//{
//    static auto path = appDataPath()  + "audio" + sep;
//    return path;
//}

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
