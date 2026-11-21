#pragma once

#include "TestResourceFs.hpp"


inline std::string load_resource(const std::string &path)
{
    const auto fs = mpc_test_resources::get_filesystem();
    const auto file = fs.open(path);
    return std::string(file.begin(), file.end());
}
