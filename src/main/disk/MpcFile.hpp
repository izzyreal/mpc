#pragma once

#include <mpc_fs.hpp>

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <variant>

namespace akaifat::fat { class AkaiFatLfnDirectoryEntry; }

/*
 * An MpcFile contains either an akaifat::fat::AkaiFatLfnDirectoryEntry or a fs::path.
 * Check each implementation for how low-level file IO is handled.
 *
 * The MpcFile sits at the same level as the AbstractDisk -- it lives in the world of the virtual MPC2000XL
 * and is directly consumable by the application for loading and saving songs, sounds, sequences, creating
 * and deleting directories, etc.
 */

namespace mpc::disk {

class StdDisk;

class MpcFile {
    
private:
    bool raw = false;
    std::shared_ptr<akaifat::fat::AkaiFatLfnDirectoryEntry> rawEntry;
    fs::path fs_path;
    
    friend class StdDisk;
        
public:
    bool isDirectory();
    bool isFile();
    bool exists();
    std::string getName();
    bool setName(std::string s);
    unsigned long length();
    void setFileData(std::vector<char>& data);
    bool del();
    std::vector<char> getBytes();
    std::vector<std::shared_ptr<MpcFile>> listFiles();
    std::string getNameWithoutExtension();
    std::string getExtension();
    std::shared_ptr<std::istream> getInputStream();
    std::shared_ptr<std::ostream> getOutputStream();
    
    explicit MpcFile(const std::variant<fs::path, std::shared_ptr<akaifat::fat::AkaiFatLfnDirectoryEntry>>&);
    
};
}
