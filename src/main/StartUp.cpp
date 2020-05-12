#include "StartUp.hpp"

#include <sys/Home.hpp>
#include <file/FileUtil.hpp>

using namespace mpc;
using namespace moduru::file;
using namespace std;

string StartUp::home = moduru::sys::Home::get();
string StartUp::resPath = home + FileUtil::getSeparator() + "vMPC" + FileUtil::getSeparator() + "resources" + FileUtil::getSeparator();
string StartUp::tempPath = home + FileUtil::getSeparator() + "vMPC" + FileUtil::getSeparator() + "temp" + FileUtil::getSeparator();
string StartUp::storesPath = home + FileUtil::getSeparator() + "vMPC" + FileUtil::getSeparator() + "Stores" + FileUtil::getSeparator();
string StartUp::logFilePath = home + FileUtil::getSeparator() + "vMPC" + FileUtil::getSeparator() + "vmpc.log";

std::string StartUp::altResPath()
{
	static auto result = moduru::sys::Home::get() + FileUtil::getSeparator() + "vMPC" + FileUtil::getSeparator() + "resources";
	return result;
}
