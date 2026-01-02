#pragma once

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(mpctest);

inline std::string load_resource(const std::string &path)
{
    const auto fs = cmrc::mpctest::get_filesystem();
    const auto file = fs.open(path);
    return std::string(file.begin(), file.end());
}

