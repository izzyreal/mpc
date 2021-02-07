#pragma once
#include <observer/Observable.hpp>
#include <disk/device/Device.hpp>
#include <disk/SoundSaver.hpp>

#include <vector>
#include <string>

/*
 * Children of AbstractDisk provide a direct interface to the rest of the application to perform
 * file operations. These operations are mostly domain-specific implementations of create, read,
 * write and rename.
 *
 * Lower level functionality like opening file handles and reading from and writing to file streams
 * is delegated to the moduru::file library by the MpcFiles that AbstractDisk uses.
 */

namespace mpc { class Mpc; }

namespace mpc::sequencer {
class Sequence;
}

namespace mpc::sampler {
class Program;
class Sound;
}

namespace mpc::disk {
class MpcFile;
class Store;
}

namespace mpc::disk {

class AbstractDisk
: public moduru::observer::Observable
{
    
private:
    bool busy = false;
    std::weak_ptr<Store> store;
    std::unique_ptr<SoundSaver> soundSaver;
    
protected:
    mpc::Mpc& mpc;
    std::unique_ptr<mpc::disk::device::Device> device;
    
public:
    const std::vector<std::string> extensions{ "", "SND", "PGM", "APS", "MID", "ALL", "WAV", "SEQ", "SET" };
    std::vector<std::shared_ptr<MpcFile>> files;
    std::vector<std::shared_ptr<MpcFile>> allFiles;
    std::vector<std::shared_ptr<MpcFile>> parentFiles;
    
public:
    static std::string formatFileSize(int size);
    static std::string padFileName16(std::string s);
    
    bool renameSelectedFile(std::string s);
    bool deleteSelectedFile();
    
    std::vector<std::string> getFileNames();
    std::string getFileName(int i);
    std::vector<std::string> getParentFileNames();
    
    std::shared_ptr<MpcFile> getFile(int i);
    std::shared_ptr<MpcFile> getFile(const std::string& fileName);
    std::vector<std::shared_ptr<MpcFile>>& getFiles(); // filtered by extension
    std::vector<std::shared_ptr<MpcFile>>& getAllFiles();
    std::shared_ptr<MpcFile> getParentFile(int i);
    std::vector<std::shared_ptr<MpcFile>>& getParentFiles();
    
    void writeSound(std::weak_ptr<mpc::sampler::Sound>);
    void writeWav(std::weak_ptr<mpc::sampler::Sound>);
    void writeSound(std::weak_ptr<mpc::sampler::Sound>, std::weak_ptr<MpcFile>);
    void writeWav(std::weak_ptr<mpc::sampler::Sound>, std::weak_ptr<MpcFile>);
    void writeSequence(std::weak_ptr<mpc::sequencer::Sequence>, std::string fileName);
    bool checkExists(std::string fileName);
    void writeProgram(std::weak_ptr<mpc::sampler::Program> program, const std::string& fileName);
    std::weak_ptr<mpc::disk::Store> getStore();
    
    void setBusy(bool);
    bool isBusy();
    void flush();
    void close();
    bool isRoot();
    
    virtual void initFiles() = 0;
    virtual std::shared_ptr<MpcFile> newFile(const std::string& name) = 0;
    virtual std::string getDirectoryName() = 0;
    virtual bool moveBack() = 0;
    virtual bool moveForward(const std::string& directoryName) = 0;
    virtual bool deleteAllFiles(int dwGuiDelete) = 0;
    virtual bool newFolder(const std::string& newDirName) = 0;
    virtual bool deleteDir(std::weak_ptr<MpcFile>) = 0;
    virtual std::string getAbsolutePath() = 0;
    
protected:
    virtual int getPathDepth() = 0;
    
protected:
    AbstractDisk(mpc::Mpc& mpc, std::weak_ptr<Store> store);    
};
}
