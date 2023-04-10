#pragma once

#include <vector>

#include <ghc/filesystem.hpp>
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (defined(__cplusplus) && __cplusplus >= 201703L)) && defined(__has_include)
#if __has_include(<filesystem>) && (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500)
#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#if !defined(__linux__) && !TARGET_OS_IOS
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#endif

#ifndef GHC_USE_STD_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

inline std::vector<char> get_file_data(fs::path p)
{
    std::ifstream ifs(p, std::ios::in | std::ios::binary | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return bytes;
}

inline void set_file_data(fs::path p, const std::vector<char>& bytes)
{
    std::ofstream ofs(p, std::ios::out | std::ios::binary);
    ofs.write(bytes.data(), bytes.size());
}
