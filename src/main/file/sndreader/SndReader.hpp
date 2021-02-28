#pragma once
#include <VecUtil.hpp>
#include <file/ByteUtil.hpp>

#include <string>
#include <vector>
#include <memory>

namespace mpc::disk
{
class MpcFile;
}

namespace mpc::file::sndreader
{
class SndHeaderReader;
}

namespace mpc::file::sndreader
{
class SndReader
{
    
private:
    std::vector<char> sndFileArray;
    std::shared_ptr<SndHeaderReader> sndHeaderReader;
    
public:
    std::string getName();
    bool isMono();
    int getNumberOfFrames();
    int getSampleRate();
    int getLevel();
    int getStart();
    int getEnd();
    int getLoopLength();
    bool isLoopEnabled();
    int getTune();
    int getNumberOfBeats();
    void writeSampleData(std::vector<float>* dest);
    
public:
    std::vector<char>& getSndFileArray();
    
public:
    SndReader(mpc::disk::MpcFile* soundFile);
    
    SndReader(const std::vector<char>& loadBytes);
};
}
