#pragma once

#include <cstddef>
#include <string>

namespace mpc
{
template <std::size_t N>
std::string utf8(const char8_t (&literal)[N])
{
    return {reinterpret_cast<const char *>(literal), N - 1};
}
} // namespace mpc
