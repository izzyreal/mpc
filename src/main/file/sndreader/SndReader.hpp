#pragma once
#include <Util.hpp>
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
    bool isHeaderValid();
    bool isMono();

    int getSampleRate();
    int getLevel();
    int getStart();
    int getEnd();
    int getLoopLength();
    bool isLoopEnabled();
    int getTune();
    int getNumberOfBeats();
    void readData(std::shared_ptr<std::vector<float>>);
    
public:
    std::vector<char>& getSndFileArray();
    
public:
    SndReader(mpc::disk::MpcFile*);
    
    SndReader(const std::vector<char>& loadBytes);
};
}
