#define _CRT_SECURE_NO_WARNINGS
#include "Logger.hpp"

#include <string>

#include "mpc_fs.hpp"

using namespace mpc;

Logger::Logger()
{
	remove(path.c_str());
}
Logger Logger::l;

void Logger::setPath(std::string s) {
	path = s;
}

void Logger::log(std::string s) {
	if (path.empty()) {
		return;
	}

	auto fp = std::ofstream (path, std::ios::app | std::ios::binary);

	s += "\n";

	fp.write(s.c_str(), s.length());

	fp.close();
}
