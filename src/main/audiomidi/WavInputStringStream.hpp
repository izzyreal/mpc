#pragma once
#include <sstream>
#include <vector>

static const int RIFF_CHUNK_ID{ 1179011410 };
static const int RIFF_TYPE_ID{ 1163280727 };
static const int FMT_CHUNK_ID{ 544501094 };
static const int DATA_CHUNK_ID{ 1635017060 };

static const int EXPECTED_HEADER_SIZE = 44;
static const int EXPECTED_FMT_DATA_SIZE = 16;

std::istringstream wav_init_istringstream(char* data, int size)
{
    std::istringstream result(std::string(data, data + size), std::ios::in | std::ios::binary);
    result.unsetf(std::ios_base::skipws);
	return result;
}

int wav_get_LE(std::istringstream& stream, int numBytes)
{
    if (numBytes < 1 || numBytes > 4)
    {
        return 0;
    }

    int pos = 0;
    char buffer[4];

    for (int i = 0; i < numBytes; i++)
    {
        char c;
        stream >> buffer[i];
    }

    numBytes--;
    pos = numBytes;

    int val = buffer[pos] & 255;

    for (auto b = 0; b < numBytes; b++)
    {
        val = (val << 8) + (buffer[--pos] & 255);
    }

    return val;
}

bool wav_read_header(std::istringstream& stream, int& sampleRate, int& validBits, int& numChannels, int& numFrames)
{    
    stream.seekg(0, stream.end);

    auto tell = stream.tellg();

    if (tell < EXPECTED_HEADER_SIZE) {
        return false;
    }

    stream.seekg(0, stream.beg);

    auto riffChunkId = wav_get_LE(stream, 4);         // Offset 0
    auto mainChunkSize = wav_get_LE(stream, 4);       // Offset 4;
    auto riffTypeId = wav_get_LE(stream, 4);          // Offset 8
    auto fmtChunkId = wav_get_LE(stream, 4);          // Offset 12
    auto lengthOfFormatData = wav_get_LE(stream, 4);  // Offset 16
    auto isPCM = wav_get_LE(stream, 2) == 1;          // Offset 20
    numChannels = wav_get_LE(stream, 2);         // Offset 22
    sampleRate = wav_get_LE(stream, 4);               // Offset 24
    auto avgBytesPerSecond = wav_get_LE(stream, 4);   // Offset 28
    auto blockAlign = wav_get_LE(stream, 2);          // Offset 32
    validBits = wav_get_LE(stream, 2);           // Offset 34
    if (lengthOfFormatData != 16) {
        stream.ignore(lengthOfFormatData - 16);
    }
    auto dataChunkId = wav_get_LE(stream, 4);         // Ofset 36

    // Skip fact and smpl chunks
    const int maxRetries = 10;
    int retryCounter = 0;

    while (dataChunkId != DATA_CHUNK_ID && retryCounter++ != maxRetries)
    {
        auto factOrOtherChunkSize = wav_get_LE(stream, 4);
        stream.ignore(factOrOtherChunkSize);
        dataChunkId = wav_get_LE(stream, 4);
    }

    auto dataChunkSize = wav_get_LE(stream, 4);       // Offset 40

    if (riffChunkId != RIFF_CHUNK_ID)
    {
        return false;
    }

    if (fmtChunkId != FMT_CHUNK_ID)
    {
        return false;
    }

    //if (lengthOfFormatData != EXPECTED_FMT_DATA_SIZE) {
        //return false;
    //}

    if (!isPCM)
    {
        return false;
    }

    if (numChannels != 1 && numChannels != 2)
    {
        return false;
    }

    if (sampleRate < 11025 || sampleRate > 44100)
    {
        return false;
    }

    if (validBits != 16)
    {
        return false;
    }

    // Some wav writers don't word align properly...
    if (mainChunkSize % 2 == 1)
    {
        mainChunkSize += 1;
    }

    if (mainChunkSize + 8 != tell)
    {
        return false;
    }

    numFrames = (dataChunkSize / (validBits / 8)) / numChannels;

    return true;
}

void wav_read_bytes(std::istringstream& stream, const std::vector<char>& bytes)
{
   stream.read((char*)(&bytes[0]), bytes.size());
}
