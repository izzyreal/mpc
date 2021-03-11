#include "Paths.hpp"

#include <sys/Home.hpp>
#include <file/FileUtil.hpp>

using namespace mpc;
using namespace moduru::file;
using namespace std;

string Paths::sep = FileUtil::getSeparator();

string Paths::home()
{
	static auto home = moduru::sys::Home::get();
	return home;
}

string Paths::resPath()
{
	static auto resPath = home() + sep + "vMPC" + sep + "resources" + sep;
	return resPath;
}

string Paths::tempPath()
{
	static auto tempPath = home() + sep + "vMPC" + sep + "temp" + sep;
	return tempPath;
}

string Paths::storesPath()
{
	static auto storesPath = home() + sep + "vMPC" + sep + "Stores" + sep;
	return storesPath;
}

string Paths::logFilePath()
{
	static auto logFilePath = home() + sep + "vMPC" + sep + "vmpc.log";
	return logFilePath;
}

string Paths::recordingsPath()
{
    static auto recPath = home() + sep + "vMPC" + sep + "recordings" + sep;
    return recPath;
}
