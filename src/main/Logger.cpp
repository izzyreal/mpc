#define _CRT_SECURE_NO_WARNINGS
#include <Logger.hpp>

#include <mpc_fs.hpp>

#include <string>

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
    const auto data = get_file_data(path);
    std::vector<char> bytes;
    if (data)
    {
        bytes = *data;
    }
    bytes.insert(bytes.end(), s.begin(), s.end());
    (void) set_file_data(path, bytes);
}
