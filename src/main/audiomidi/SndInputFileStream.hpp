#pragma once

#include "file/kaitai/Mpc60SampleDecoder.hpp"

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#ifdef __linux__
#include <string.h> // For strcmp
#endif

enum class SndInputEncoding
{
    Mpc2000XlPcm16,
    Mpc3000Pcm16,
    Mpc60Packed12,
};

const char MPC2000XL_SND_ID[] = {0x01, 0x04, '\0'};
const char MPC3000_SND_ID[] = {0x01, 0x02, '\0'};
const char MPC60_SND_ID[] = {0x01, 0x01, '\0'};

const int ID_INDEX = 0; // 2 byte ANSI string of a number greater than '10' and
                        // equal to or smaller than '15'
const int NAME_INDEX = 2;             // 16 byte ISO-8859 string
const int NAME_TERMINATOR_INDEX = 18; // 0x00
const int LEVEL_INDEX = 19;           // byte
const int TUNE_INDEX = 20;            // byte
const int STEREO_INDEX = 21;          // byte, 1 if true
const int START_INDEX = 22;           // 4-byte uint
const int END_INDEX = 26;             // 4-byte uint
const int FRAME_COUNT_INDEX = 30;     // 4-byte uint
const int LOOP_LENGTH_INDEX = 34;     // 4-byte uint
const int LOOP_ENABLED_INDEX = 38;    // 1 byte, 1 if true
const int BEAT_COUNT_INDEX = 39;      // 1 byte
const int SAMPLE_RATE_INDEX = 40;     // 2 byte unsigned short
const int MPC2000XL_SND_HEADER_SIZE = 42;
const int MPC3000_SND_HEADER_SIZE = 38;
const int MPC60_SND_HEADER_SIZE = 39;

inline void snd_read_bytes(const std::shared_ptr<std::istream> &stream,
                           const std::vector<char> &bytes, const int maxLength)
{
    auto byteCountToRead = (std::min)((int)bytes.size(), maxLength);
    stream->read((char *)&bytes[0], byteCountToRead);
}

inline std::string snd_get_string(const std::shared_ptr<std::istream> &stream,
                                  const int maxLength)
{
    std::vector<char> buffer(maxLength);
    snd_read_bytes(stream, buffer, maxLength);

    for (int i = 0; i < buffer.size(); i++)
    {
        if (buffer[i] == 0x20 || buffer[i] == 0x00)
        {
            buffer = std::vector(buffer.begin(), buffer.begin() + i);
            break;
        }
    }

    return std::string(buffer.begin(), buffer.end());
}

inline uint16_t snd_get_unsigned_short_LE(
    const std::shared_ptr<std::istream> &stream)
{
    char buffer[2];
    stream->read(buffer, 2);
    return static_cast<uint16_t>((buffer[1] & 0xFF) << 8 | buffer[0] & 0xFF);
}

inline int snd_get_LE(const std::shared_ptr<std::istream> &stream,
                      int numBytes)
{

    if (numBytes < 1 || numBytes > 4)
    {
        return 0;
    }

    int pos = 0;
    char buffer[4];

    stream->read(buffer, numBytes);

    numBytes--;
    pos = numBytes;

    int val = buffer[pos] & 255;

    for (auto b = 0; b < numBytes; b++)
    {
        val = (val << 8) + (buffer[--pos] & 255);
    }

    return val;
}

inline uint32_t snd_get_unsigned_LE(
    const std::shared_ptr<std::istream> &stream, int numBytes)
{
    return static_cast<uint32_t>(snd_get_LE(stream, numBytes));
}

inline char snd_get_char(const std::shared_ptr<std::istream> &stream)
{
    char buf[1];
    stream->read(buf, 1);
    return *buf;
}

inline bool snd_read_mpc2000xl_header(
    const std::shared_ptr<std::istream> &stream, int &sampleRate,
    int &validBits, int &numChannels, int &numFrames)
{
    auto name = snd_get_string(stream, 17);

    /*auto level =*/snd_get_char(stream);
    /*auto tune =*/snd_get_char(stream);
    auto stereo = snd_get_char(stream) == 1;

    numChannels = stereo ? 2 : 1;

    /*auto start =*/snd_get_LE(stream, 4);
    /*auto end =*/snd_get_LE(stream, 4);

    numFrames = snd_get_LE(stream, 4);

    /*auto loopLength =*/snd_get_LE(stream, 4);
    /*auto loopEnabled =*/snd_get_char(stream) /*== 1*/;
    /*auto beatCount =*/snd_get_char(stream);
    sampleRate = snd_get_unsigned_short_LE(stream);

    validBits = 16;

    if (numChannels != 1 && numChannels != 2)
    {
        return false;
    }

    if (sampleRate < 11025 || sampleRate > 65000)
    {
        return false;
    }

    return true;
}

inline bool snd_read_mpc60_header(const std::shared_ptr<std::istream> &stream,
                                  int &sampleRate, int &validBits,
                                  int &numChannels, int &numFrames)
{
    stream->seekg(0, std::ios::end);
    const auto fileSize = stream->tellg();

    if (fileSize < MPC60_SND_HEADER_SIZE)
    {
        return false;
    }

    stream->seekg(2, std::ios::beg);

    auto name = snd_get_string(stream, 17);
    numFrames = static_cast<int>(snd_get_unsigned_LE(stream, 4));
    /*auto startMsec =*/snd_get_unsigned_LE(stream, 2);
    /*auto endMsec =*/snd_get_unsigned_LE(stream, 4);
    /*auto decayMsec =*/snd_get_unsigned_LE(stream, 2);
    /*auto volume =*/snd_get_char(stream);
    /*auto tuning =*/snd_get_char(stream);
    stream->ignore(5);
    /*auto velocityToVolume =*/snd_get_char(stream);

    const auto packedByteCount = static_cast<std::streamoff>(
        ((static_cast<uint64_t>(numFrames) + 1U) / 2U) * 3U);

    if (numFrames <= 0 ||
        fileSize != MPC60_SND_HEADER_SIZE + packedByteCount)
    {
        return false;
    }

    sampleRate = mpc::file::kaitai::kMpc60SampleRate;
    validBits = 16;
    numChannels = 1;
    stream->seekg(MPC60_SND_HEADER_SIZE, std::ios::beg);
    return true;
}

inline bool snd_read_mpc3000_header(const std::shared_ptr<std::istream> &stream,
                                    int &sampleRate, int &validBits,
                                    int &numChannels, int &numFrames)
{
    stream->seekg(0, std::ios::end);
    const auto fileSize = stream->tellg();

    if (fileSize < MPC3000_SND_HEADER_SIZE)
    {
        return false;
    }

    stream->seekg(2, std::ios::beg);

    auto name = snd_get_string(stream, 17);
    /*auto level =*/snd_get_char(stream);
    stream->ignore(2);
    /*auto start =*/snd_get_unsigned_LE(stream, 4);
    /*auto end =*/snd_get_unsigned_LE(stream, 4);
    numFrames = static_cast<int>(snd_get_unsigned_LE(stream, 4));
    /*auto unknown2 =*/snd_get_unsigned_LE(stream, 4);

    if (numFrames <= 0 ||
        fileSize != MPC3000_SND_HEADER_SIZE + static_cast<std::streamoff>(numFrames * 2))
    {
        return false;
    }

    sampleRate = 44100;
    validBits = 16;
    numChannels = 1;
    stream->seekg(MPC3000_SND_HEADER_SIZE, std::ios::beg);
    return true;
}

inline bool snd_read_header(const std::shared_ptr<std::istream> &stream,
                            int &sampleRate, int &validBits,
                            int &numChannels, int &numFrames,
                            SndInputEncoding &encoding)
{
    stream->clear();
    stream->seekg(0, std::ios::beg);

    const auto sndId = snd_get_string(stream, 2);

    if (strcmp(sndId.c_str(), MPC2000XL_SND_ID) == 0)
    {
        encoding = SndInputEncoding::Mpc2000XlPcm16;
        return snd_read_mpc2000xl_header(stream, sampleRate, validBits,
                                         numChannels, numFrames);
    }

    if (strcmp(sndId.c_str(), MPC3000_SND_ID) == 0)
    {
        encoding = SndInputEncoding::Mpc3000Pcm16;
        return snd_read_mpc3000_header(stream, sampleRate, validBits,
                                       numChannels, numFrames);
    }

    if (strcmp(sndId.c_str(), MPC60_SND_ID) == 0)
    {
        encoding = SndInputEncoding::Mpc60Packed12;
        return snd_read_mpc60_header(stream, sampleRate, validBits,
                                     numChannels, numFrames);
    }

    return false;
}

inline bool snd_read_header(const std::shared_ptr<std::istream> &stream,
                            int &sampleRate, int &validBits,
                            int &numChannels, int &numFrames)
{
    SndInputEncoding encoding;
    return snd_read_header(stream, sampleRate, validBits, numChannels,
                           numFrames, encoding);
}
