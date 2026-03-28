#define _CRT_SECURE_NO_WARNINGS
#include <Logger.hpp>

#include <mpc_fs.hpp>

#include <string>
#include <fstream>

using namespace mpc;

Logger::Logger()
{
    remove(path.c_str());
}
Logger Logger::l;

void Logger::setPath(const std::string &s)
{
    path = s;
}

void Logger::log(std::string s) const
{
    if (path.empty())
    {
        return;
    }

    s += "\n";

    auto fp = std::ofstream(path, std::ios::app | std::ios::binary);
    if (!fp.is_open())
    {
        return;
    }

    fp.write(s.c_str(), s.length());
}
