#pragma once
#include <disk/AbstractDisk.hpp>
#include <file/File.hpp>

#include <memory>
#include <string>
#include <vector>

namespace moduru {
namespace file {
class File;
class Directory;
class FsNode;
}
}

namespace mpc::disk
{
namespace device {
class Device;
}

class Volume;
class MpcFile;

class StdDisk
: public AbstractDisk
{
private:
    std::vector<std::string> path;
    std::shared_ptr<moduru::file::Directory> root;
    
private:
    void initParentFiles();
    
public:
    void initFiles() override;
    std::string getDirectoryName() override;
    bool moveBack() override;
    bool moveForward(const std::string& directoryName) override;
    std::shared_ptr<moduru::file::Directory> getDir();
    
private:
    std::shared_ptr<moduru::file::Directory> getParentDir();
    void renameFilesToAkai();
    
protected:
    int getPathDepth() override;
    
public:
    bool deleteAllFiles(int dwGuiDelete) override;
    bool newFolder(const std::string& newDirName) override;
    bool deleteDir(std::weak_ptr<MpcFile> f) override;
    
private:
    bool deleteRecursive(std::weak_ptr<MpcFile>);
    
public:
    std::shared_ptr<MpcFile> newFile(const std::string& newFileName) override;
    std::string getAbsolutePath() override;
    
    StdDisk(mpc::Mpc&, Volume&);
    
};
}
