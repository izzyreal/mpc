#pragma once

#include <ios>
#include <vector>
#include <cmath>
#include <system_error>

#include <ghc/filesystem.hpp>
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) ||                         \
     (defined(__cplusplus) && __cplusplus >= 201703L)) &&                      \
    defined(__has_include)
#if __has_include(<filesystem>) && (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500)
#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#if !defined(__linux__) && !TARGET_OS_IOS
#define GHC_USE_STD_FS
#include <filesystem>
#endif
#endif
#endif

namespace mpc_fs
{
namespace detail
{
#ifdef GHC_USE_STD_FS
namespace fs = std::filesystem;
#else
namespace fs = ghc::filesystem;
#endif
} // namespace detail

using path = detail::fs::path;
using directory_iterator = detail::fs::directory_iterator;
using recursive_directory_iterator = detail::fs::recursive_directory_iterator;
using directory_entry = detail::fs::directory_entry;
using file_time_type = detail::fs::file_time_type;
using space_info = detail::fs::space_info;
using filesystem_error = detail::fs::filesystem_error;

inline bool create_directories(const path &p)
{
    return detail::fs::create_directories(p);
}

inline bool create_directories(const path &p, std::error_code &ec)
{
    return detail::fs::create_directories(p, ec);
}

inline bool create_directory(const path &p)
{
    return detail::fs::create_directory(p);
}

inline bool create_directory(const path &p, std::error_code &ec)
{
    return detail::fs::create_directory(p, ec);
}

inline bool exists(const path &p)
{
    return detail::fs::exists(p);
}

inline bool exists(const path &p, std::error_code &ec)
{
    return detail::fs::exists(p, ec);
}

inline bool is_directory(const path &p)
{
    return detail::fs::is_directory(p);
}

inline bool is_directory(const directory_entry &entry)
{
    return detail::fs::is_directory(entry);
}

inline bool is_regular_file(const path &p)
{
    return detail::fs::is_regular_file(p);
}

inline bool is_regular_file(const path &p, std::error_code &ec)
{
    return detail::fs::is_regular_file(p, ec);
}

inline bool is_regular_file(const directory_entry &entry)
{
    return detail::fs::is_regular_file(entry);
}

inline std::uintmax_t file_size(const path &p)
{
    return detail::fs::file_size(p);
}

inline std::uintmax_t file_size(const path &p, std::error_code &ec) noexcept
{
    return detail::fs::file_size(p, ec);
}

inline file_time_type last_write_time(const path &p)
{
    return detail::fs::last_write_time(p);
}

inline path relative(const path &p, const path &base)
{
    return detail::fs::relative(p, base);
}

inline path relative(const path &p, const path &base, std::error_code &ec)
{
    return detail::fs::relative(p, base, ec);
}

inline bool remove(const path &p)
{
    return detail::fs::remove(p);
}

inline bool remove(const path &p, std::error_code &ec)
{
    return detail::fs::remove(p, ec);
}

inline std::uintmax_t remove_all(const path &p)
{
    return detail::fs::remove_all(p);
}

inline std::uintmax_t remove_all(const path &p, std::error_code &ec)
{
    return detail::fs::remove_all(p, ec);
}

inline void rename(const path &from, const path &to)
{
    detail::fs::rename(from, to);
}

inline void rename(const path &from, const path &to, std::error_code &ec)
{
    detail::fs::rename(from, to, ec);
}

inline space_info space(const path &p)
{
    return detail::fs::space(p);
}

inline space_info space(const path &p, std::error_code &ec) noexcept
{
    return detail::fs::space(p, ec);
}

inline path temp_directory_path()
{
    return detail::fs::temp_directory_path();
}

inline path temp_directory_path(std::error_code &ec)
{
    return detail::fs::temp_directory_path(ec);
}
} // namespace mpc_fs

inline std::vector<char> get_file_data(const mpc_fs::path &p)
{
    std::ifstream ifs(p, std::ios::in | std::ios::binary | std::ios::ate);

    if (!ifs.is_open())
    {
        return {};
    }

    const std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(static_cast<size_t>(fileSize));
    ifs.read(bytes.data(), fileSize);

    return bytes;
}

inline void set_file_data(const mpc_fs::path &p, const std::vector<char> &bytes)
{
    std::ofstream ofs(p, std::ios::out | std::ios::binary);
    ofs.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}

inline void set_file_data(const mpc_fs::path &p, const std::string &bytes)
{
    std::ofstream ofs(p, std::ios::out | std::ios::binary);
    ofs.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}

inline std::string
byte_count_to_short_string(const uintmax_t byte_count,
                           const bool one_letter_suffix = false)
{
    const static std::vector<std::string> units = {"B",  "KB", "MB", "GB", "TB",
                                                   "PB", "EB", "ZB", "YB"};
    size_t unit = 0;

    auto adjustedSize = byte_count;
#ifdef __APPLE__
    constexpr auto denominator = 1000;
#else
    const auto denominator = 1024;
#endif

    while (adjustedSize >= denominator && unit < units.size() - 1)
    {
        unit++;
        adjustedSize /= denominator;
    }

    return std::to_string(static_cast<int>(std::floor(adjustedSize))) +
           (one_letter_suffix ? units[unit].substr(0, 1) : units[unit]);
}
