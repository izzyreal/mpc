#pragma once
#include <disk/AbstractDisk.hpp>
#include <disk/Volume.hpp>

#include <fat/AkaiFatLfnDirectory.hpp>
#include <fat/AkaiFatLfnDirectoryEntry.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc::disk {

class MpcFile;

class RawDisk
: public AbstractDisk
{
public:
    RawDisk(mpc::Mpc&);
    
private:
    void initParentFiles();
    Volume volume;
    std::vector<std::shared_ptr<akaifat::fat::AkaiFatLfnDirectoryEntry>> path;
    std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory> root;
    std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory> getDir();
    std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory> getParentDir();
    bool deleteFilesRecursive(std::shared_ptr<akaifat::fat::AkaiFatLfnDirectoryEntry> entry);
    bool deleteDirsRecursive(std::shared_ptr<akaifat::fat::AkaiFatLfnDirectoryEntry> entry, bool checkExist);
    void refreshPath();

public:
    void initFiles() override;
    std::string getDirectoryName() override;
    bool moveBack() override;
    bool moveForward(const std::string& directoryName) override;
    void close() override;
    void flush() override;
    bool deleteAllFiles(int dwGuiDelete) override;
    bool newFolder(const std::string& newDirName) override;
    bool deleteDir(std::weak_ptr<MpcFile> f) override;
    std::shared_ptr<MpcFile> newFile(const std::string& newFileName) override;
    std::string getAbsolutePath() override;
    std::string getTypeShortName() override;
    std::string getModeShortName() override;
    uint64_t getTotalSize() override;
    std::string getVolumeLabel() override;
    Volume& getVolume() override { return volume; }
    void initRoot() override { if (root) return; root = volume.getRawRoot(); }
    
protected:
    int getPathDepth() override;

};
}
