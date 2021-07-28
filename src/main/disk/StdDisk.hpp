#pragma once
#include <disk/AbstractDisk.hpp>
#include <file/File.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mpc::disk {
class Volume;
class MpcFile;

class StdDisk
: public AbstractDisk
{
    
public:
    StdDisk(mpc::Mpc&, Volume&);
    std::shared_ptr<MpcFile> getDir();
    
private:
    std::shared_ptr<mpc::disk::MpcFile> root;
    Volume& volume;
    std::vector<std::string> path;
    void initParentFiles();
    std::shared_ptr<MpcFile> getParentDir();
    void renameFilesToAkai();
    bool deleteRecursive(std::weak_ptr<MpcFile>);
    
public:
    void initFiles() override;
    std::string getDirectoryName() override;
    bool moveBack() override;
    bool moveForward(const std::string& directoryName) override;
    void close() override;
    void flush() override;
    Volume& getStore() override;
    bool deleteAllFiles(int dwGuiDelete) override;
    bool newFolder(const std::string& newDirName) override;
    bool deleteDir(std::weak_ptr<MpcFile> f) override;
    std::shared_ptr<MpcFile> newFile(const std::string& newFileName) override;
    std::string getAbsolutePath() override;
    
protected:
    int getPathDepth() override;
        
};
}
