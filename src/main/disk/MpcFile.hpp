#pragma once

#include <thirdp/any.hpp>

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
    //::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* rawEntry{};
    std::shared_ptr<moduru::file::FsNode> stdEntry;
    
public:
    bool isStd();
    bool isDirectory();
    std::string getName();
    bool setName(std::string s);
    int length();
    void setFileData(std::vector<char>* data);
    bool del();
    std::weak_ptr<moduru::file::FsNode> getFsNode();
    std::weak_ptr<moduru::file::File> getFile();
    //::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* getEntry();
    std::vector<char> getBytes();
    
    MpcFile(nonstd::any a);
    
};
}
