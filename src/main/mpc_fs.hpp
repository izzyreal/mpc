#pragma once

#include <ios>
#include <vector>
#include <cmath>
#include <system_error>
#include <string>

#include <ghc/filesystem.hpp>
#include <Logger.hpp>
#include <tl/expected.hpp>
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

struct fs_error
{
    std::string operation;
    path path1;
    path path2;
    bool has_path2 = false;
    std::error_code ec;
    std::string message;
};

template <typename T>
using result = tl::expected<T, fs_error>;

inline fs_error make_error(const std::string &operation, const path &path1,
                           const std::error_code &ec,
                           const path *path2 = nullptr)
{
    fs_error error;
    error.operation = operation;
    error.path1 = path1;
    if (path2 != nullptr)
    {
        error.path2 = *path2;
        error.has_path2 = true;
    }
    error.ec = ec;
    error.message = ec.message();

    auto msg = "mpc_fs::" + operation + " failed";
    if (!path1.empty())
    {
        msg += " path1='" + path1.string() + "'";
    }
    if (path2 != nullptr && !path2->empty())
    {
        msg += " path2='" + path2->string() + "'";
    }
    if (ec)
    {
        msg += " ec=" + std::to_string(ec.value()) + " (" + ec.message() + ")";
    }
    MLOG(msg);

    return error;
}

inline fs_error make_exception_error(const std::string &operation, const path &path1,
                                     const std::exception &exception,
                                     const path *path2 = nullptr)
{
    fs_error error;
    error.operation = operation;
    error.path1 = path1;
    if (path2 != nullptr)
    {
        error.path2 = *path2;
        error.has_path2 = true;
    }
    error.message = exception.what();
    MLOG("mpc_fs::" + operation + " threw exception: " + error.message);
    return error;
}

inline fs_error make_unknown_exception_error(const std::string &operation,
                                             const path &path1,
                                             const path *path2 = nullptr)
{
    fs_error error;
    error.operation = operation;
    error.path1 = path1;
    if (path2 != nullptr)
    {
        error.path2 = *path2;
        error.has_path2 = true;
    }
    error.message = "unknown exception";
    MLOG("mpc_fs::" + operation + " threw unknown exception");
    return error;
}

[[nodiscard]] inline result<bool> create_directories(const path &p)
{
    std::error_code ec;
    const auto created = detail::fs::create_directories(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("create_directories", p, ec));
    }
    return created;
}

[[nodiscard]] inline result<bool> create_directory(const path &p)
{
    std::error_code ec;
    const auto created = detail::fs::create_directory(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("create_directory", p, ec));
    }
    return created;
}

[[nodiscard]] inline result<directory_iterator>
make_directory_iterator(const path &p)
{
    std::error_code ec;
    directory_iterator it(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("directory_iterator", p, ec));
    }
    return it;
}

[[nodiscard]] inline result<recursive_directory_iterator>
make_recursive_directory_iterator(const path &p)
{
    std::error_code ec;
    recursive_directory_iterator it(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("recursive_directory_iterator", p, ec));
    }
    return it;
}

inline directory_iterator directory_end()
{
    return {};
}

inline recursive_directory_iterator recursive_directory_end()
{
    return {};
}

[[nodiscard]] inline result<bool> exists(const path &p)
{
    std::error_code ec;
    const auto value = detail::fs::exists(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("exists", p, ec));
    }
    return value;
}

[[nodiscard]] inline result<bool> is_directory(const path &p)
{
    std::error_code ec;
    const auto value = detail::fs::is_directory(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("is_directory", p, ec));
    }
    return value;
}

[[nodiscard]] inline result<bool> is_directory(const directory_entry &entry)
{
    try
    {
        return detail::fs::is_directory(entry);
    }
    catch (const std::exception &e)
    {
        return tl::unexpected(
            make_exception_error("is_directory(directory_entry)", entry.path(), e));
    }
    catch (...)
    {
        return tl::unexpected(
            make_unknown_exception_error("is_directory(directory_entry)",
                                         entry.path()));
    }
}

[[nodiscard]] inline result<bool> is_regular_file(const path &p)
{
    std::error_code ec;
    const auto value = detail::fs::is_regular_file(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("is_regular_file", p, ec));
    }
    return value;
}

[[nodiscard]] inline result<bool> is_regular_file(const directory_entry &entry)
{
    try
    {
        return detail::fs::is_regular_file(entry);
    }
    catch (const std::exception &e)
    {
        return tl::unexpected(make_exception_error("is_regular_file(directory_entry)",
                                                   entry.path(), e));
    }
    catch (...)
    {
        return tl::unexpected(
            make_unknown_exception_error("is_regular_file(directory_entry)",
                                         entry.path()));
    }
}

[[nodiscard]] inline result<std::uintmax_t> file_size(const path &p)
{
    std::error_code ec;
    const auto value = detail::fs::file_size(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("file_size", p, ec));
    }
    return value;
}

[[nodiscard]] inline result<file_time_type> last_write_time(const path &p)
{
    std::error_code ec;
    const auto value = detail::fs::last_write_time(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("last_write_time", p, ec));
    }
    return value;
}

[[nodiscard]] inline result<path> relative(const path &p, const path &base)
{
    std::error_code ec;
    const auto value = detail::fs::relative(p, base, ec);
    if (ec)
    {
        return tl::unexpected(make_error("relative", p, ec, &base));
    }
    return value;
}

[[nodiscard]] inline result<bool> remove(const path &p)
{
    std::error_code ec;
    const auto value = detail::fs::remove(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("remove", p, ec));
    }
    return value;
}

[[nodiscard]] inline result<std::uintmax_t> remove_all(const path &p)
{
    std::error_code ec;
    const auto value = detail::fs::remove_all(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("remove_all", p, ec));
    }
    return value;
}

[[nodiscard]] inline result<void> rename(const path &from, const path &to)
{
    std::error_code ec;
    detail::fs::rename(from, to, ec);
    if (ec)
    {
        return tl::unexpected(make_error("rename", from, ec, &to));
    }
    return {};
}

[[nodiscard]] inline result<space_info> space(const path &p)
{
    std::error_code ec;
    const auto value = detail::fs::space(p, ec);
    if (ec)
    {
        return tl::unexpected(make_error("space", p, ec));
    }
    return value;
}

[[nodiscard]] inline result<path> temp_directory_path()
{
    std::error_code ec;
    const auto value = detail::fs::temp_directory_path(ec);
    if (ec)
    {
        return tl::unexpected(make_error("temp_directory_path", {}, ec));
    }
    return value;
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
