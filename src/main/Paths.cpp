#include "Paths.hpp"

#include <sys/Home.hpp>
#include <file/FileUtil.hpp>

using namespace mpc;
using namespace moduru::file;
using namespace std;

string Paths::home()
{
	static auto home = moduru::sys::Home::get();
	return home;
}

string Paths::resPath()
{
	static auto resPath = home() + FileUtil::getSeparator() + "vMPC" + FileUtil::getSeparator() + "resources" + FileUtil::getSeparator();
	return resPath;
}

string Paths::tempPath()
{
	static auto tempPath = home() + FileUtil::getSeparator() + "vMPC" + FileUtil::getSeparator() + "temp" + FileUtil::getSeparator();
	return tempPath;
}

string Paths::storesPath()
{
	static auto storesPath = home() + FileUtil::getSeparator() + "vMPC" + FileUtil::getSeparator() + "Stores" + FileUtil::getSeparator();
	return storesPath;
}

string Paths::logFilePath()
{
	static auto logFilePath = home() + FileUtil::getSeparator() + "vMPC" + FileUtil::getSeparator() + "vmpc.log";
	return logFilePath;
}
