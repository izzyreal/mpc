#pragma once
#include <disk/SoundSaver.hpp>

#include <vector>
#include <string>

#include <mpc_types.hpp>

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
class Volume;

class AbstractDisk
{
private:
    std::unique_ptr<SoundSaver> soundSaver;
    
protected:
    AbstractDisk(mpc::Mpc&);

    mpc::Mpc& mpc;
    const std::vector<std::string> extensions{ "", "SND", "PGM", "APS", "MID", "ALL", "WAV", "SEQ", "SET" };
    std::vector<std::shared_ptr<MpcFile>> files;
    std::vector<std::shared_ptr<MpcFile>> allFiles;
    std::vector<std::shared_ptr<MpcFile>> parentFiles;

    virtual int getPathDepth() = 0;

public:
    bool deleteSelectedFile();
    
    std::vector<std::string> getFileNames();
    std::string getFileName(int i);
    std::vector<std::string> getParentFileNames();
    
    std::shared_ptr<MpcFile> getFile(int i);
    std::shared_ptr<MpcFile> getFile(const std::string& fileName);
    std::vector<std::shared_ptr<MpcFile>>& getAllFiles();
    std::shared_ptr<MpcFile> getParentFile(int i);
    
    void writeSound(std::weak_ptr<mpc::sampler::Sound>);
    void writeWav(std::weak_ptr<mpc::sampler::Sound>);
    void writeSound(std::weak_ptr<mpc::sampler::Sound>, std::weak_ptr<MpcFile>);
    void writeWav(std::weak_ptr<mpc::sampler::Sound>, std::weak_ptr<MpcFile>);
    void writeSequence(std::weak_ptr<mpc::sequencer::Sequence>, std::string fileName);
    bool checkExists(std::string fileName);
    void writeProgram(std::weak_ptr<mpc::sampler::Program> program, const std::string& fileName);
    bool deleteRecursive(std::weak_ptr<MpcFile>);
    
    bool isRoot();
    
    virtual void flush() = 0;
    virtual void close() = 0;
    virtual void initFiles() = 0;
    virtual std::shared_ptr<MpcFile> newFile(const std::string& name) = 0;
    virtual std::string getDirectoryName() = 0;
    virtual bool moveBack() = 0;
    virtual bool moveForward(const std::string& directoryName) = 0;
    virtual bool deleteAllFiles(int dwGuiDelete) = 0;
    virtual bool newFolder(const std::string& newDirName) = 0;
    virtual std::string getAbsolutePath() = 0;
    virtual std::string getTypeShortName() = 0;
    virtual std::string getModeShortName() = 0;
    virtual uint64_t getTotalSize() = 0;
    virtual std::string getVolumeLabel() = 0;
    virtual Volume& getVolume() = 0;
    virtual void initRoot() = 0;

};
}
