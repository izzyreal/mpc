#pragma once

#include <thirdp/any.hpp>

#include <fat/AkaiFatLfnDirectoryEntry.hpp>

#include <vector>
#include <string>
#include <memory>

/*
 * An MpcFile contains either an AkaiFatLongFileNameDirectoryEntry or a moduru::file::FileSystemNode.
 * Check each implementation for how low-level file IO is handled.
 *
 * The MpcFile sits at the same level as the AbstractDisk -- it lives in the world of the virtual MPC2000XL
 * and is directly consumable by the application for loading and saving songs, sounds, sequences, creating
 * and deleting directories, etc.
 */

namespace moduru::file
{
class FsNode;
class File;
}

namespace mpc::disk
{
class MpcFile
{
    
private:
    bool raw = false;
    std::shared_ptr<akaifat::fat::AkaiFatLfnDirectoryEntry> rawEntry;
    std::shared_ptr<moduru::file::FsNode> stdNode;
    
public:
    bool isStd();
    bool isDirectory();
    bool exists();
    std::string getName();
    bool setName(std::string s);
    int length();
    void setFileData(std::vector<char>* data);
    bool del();
    std::weak_ptr<moduru::file::File> getFile();
    std::vector<char> getBytes();
    std::vector<std::shared_ptr<MpcFile>> listFiles();
    std::string getNameWithoutExtension();
    
    MpcFile(nonstd::any a);
    
};
}
