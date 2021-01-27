#pragma once
#include <fstream>
#include <vector>

#include <file/FileUtil.hpp>

const char ID[] = { 0x01, 0x04, '\0' };

const int ID_INDEX = 0; // 2 byte ANSI string of a number greater than '10' and equal to or smaller than '15'
const int NAME_INDEX = 2; // 16 byte ISO-8859 string
const int NAME_TERMINATOR_INDEX = 18; // 0x00
const int LEVEL_INDEX = 19; // byte
const int TUNE_INDEX = 20; // byte
const int STEREO_INDEX = 21; // byte, 1 if true
const int START_INDEX = 22; // 4-byte uint
const int END_INDEX = 26; // 4-byte uint
const int FRAME_COUNT_INDEX = 30; // 4-byte uint
const int LOOP_LENGTH_INDEX = 34; // 4-byte uint
const int LOOP_ENABLED_INDEX = 38; // 1 byte, 1 if true
const int BEAT_COUNT_INDEX = 39; // 1 byte
const int SAMPLE_RATE_INDEX = 40; // 2 byte unsigned short

std::ifstream snd_init_ifstream(const std::string& path)
{
    auto result = moduru::file::FileUtil::ifstreamw(path.c_str(), std::ios::in | std::ios::binary);
    result.unsetf(std::ios_base::skipws);
    return result;
}

void snd_read_bytes(std::ifstream& stream, const std::vector<char>& bytes, const int maxLength)
{
    auto byteCountToRead = std::min((int)bytes.size(), maxLength);
    stream.read((char*)(&bytes[0]), byteCountToRead);
}

std::string snd_get_string(std::ifstream& stream, const int maxLength)
{
    std::vector<char> buffer(17);
    snd_read_bytes(stream, buffer, maxLength);

    if (buffer.size() > maxLength)
    {
        buffer = std::vector<char>(buffer.begin(), buffer.begin() + maxLength);
    }

    for (int i = 0; i < buffer.size(); i++)
    {
        if (buffer[i] == 0x20 || buffer[i] == 0x00)
        {
            buffer = std::vector<char>(buffer.begin(), buffer.begin() + i);
            break;
        }
    }

    return std::string(buffer.begin(), buffer.end());
}

uint16_t snd_get_unsigned_short_LE(std::ifstream& stream)
{
    unsigned char buffer[2];
    stream >> buffer[0];
    stream >> buffer[1];
    return static_cast<uint16_t>((buffer[1] << 8) | buffer[0]);
}


int snd_get_LE(std::ifstream& stream, int numBytes)
{

    if (numBytes < 1 || numBytes > 4) {
        return 0;
    }

    int pos = 0;
    char buffer[4];

    for (int i = 0; i < numBytes; i++) {
        stream >> buffer[i];
    }

    numBytes--;
    pos = numBytes;

    int val = buffer[pos] & 255;

    for (auto b = 0; b < numBytes; b++) {
        val = (val << 8) + (buffer[--pos] & 255);
    }

    return val;
}

char snd_get_char(std::ifstream& stream)
{
    char result;
    stream >> result;
    return result;
}

bool snd_read_header(std::ifstream& stream, int& sampleRate, int& validBits, int& numChannels, int& numFrames)
{
    stream.seekg(0, stream.end);
    auto tell = stream.tellg();
    stream.seekg(0, stream.beg);
    
    auto sndId = snd_get_string(stream, 2);
    
    if (strcmp(sndId.c_str(), ID) != 0) {
        return false;
    }

    auto name = snd_get_string(stream, 17);
    
    auto level = snd_get_char(stream);
    auto tune = snd_get_char(stream);
    auto stereo = snd_get_char(stream) == 1;

    numChannels = stereo ? 2 : 1;

    auto start = snd_get_LE(stream, 4);
    auto end = snd_get_LE(stream, 4);
    
    numFrames = snd_get_LE(stream, 4);

    auto loopLength = snd_get_LE(stream, 4);
    auto loopEnabled = snd_get_char(stream) == 1;
    auto beatCount = snd_get_char(stream);
    sampleRate = snd_get_unsigned_short_LE(stream);

    validBits = 16;

    if (numChannels != 1 && numChannels != 2) {
        return false;
    }

    if (sampleRate < 11025 || sampleRate > 48000) {
        return false;
    }

    return true;
}

void snd_close(std::ifstream& stream)
{
    stream.close();
}
