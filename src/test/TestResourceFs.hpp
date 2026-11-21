#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace mpc_test_resources
{
class File
{
public:
    explicit File(std::string bytes) : bytes_(std::move(bytes)) {}

    const char* begin() const { return bytes_.data(); }
    const char* end() const { return bytes_.data() + bytes_.size(); }
    std::size_t size() const { return bytes_.size(); }

private:
    std::string bytes_;
};

class DirectoryEntry
{
public:
    DirectoryEntry(std::string name, bool directory)
        : name_(std::move(name)), directory_(directory) {}

    const std::string& filename() const { return name_; }
    bool is_directory() const { return directory_; }

private:
    std::string name_;
    bool directory_;
};

class FileSystem
{
public:
    File open(const std::string& path) const;
    std::vector<DirectoryEntry> iterate_directory(const std::string& path) const;
};

FileSystem get_filesystem();
} // namespace mpc_test_resources
