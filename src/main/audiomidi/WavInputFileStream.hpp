#pragma once
#include <istream>
#include <vector>

static const int RIFF_CHUNK_ID{ 1179011410 };
static const int RIFF_TYPE_ID{ 1163280727 };
static const int FMT_CHUNK_ID{ 544501094 };
static const int DATA_CHUNK_ID{ 1635017060 };

static const int EXPECTED_HEADER_SIZE = 44;
//static const int EXPECTED_FMT_DATA_SIZE = 16;

int wav_get_LE(const std::shared_ptr<std::istream>& stream, int numBytes)
{
    if (numBytes < 1 || numBytes > 4)
    {
        return 0;
    }

    char buffer[4];

    stream->read(buffer, numBytes);
    
    numBytes--;
    auto pos = numBytes;

    int val = buffer[pos] & 255;

    for (auto b = 0; b < numBytes; b++)
        val = (val << 8) + (buffer[--pos] & 255);

    return val;
}

bool wav_read_header(const std::shared_ptr<std::istream>& stream, int& sampleRate, int& validBits, int& numChannels, int& numFrames)
{    
    stream->seekg(0, stream->end);

    auto tell = stream->tellg();

    if (tell < EXPECTED_HEADER_SIZE) {
        return false;
    }

    stream->seekg(0, stream->beg);

    auto riffChunkId = wav_get_LE(stream, 4);         // Offset 0
    auto mainChunkSize = wav_get_LE(stream, 4);       // Offset 4;
    /*auto riffTypeId =*/ wav_get_LE(stream, 4);          // Offset 8
    auto fmtChunkId = wav_get_LE(stream, 4);          // Offset 12

    while (fmtChunkId != FMT_CHUNK_ID && stream->gcount() >= 4)
    {
        const auto chunkSize = wav_get_LE(stream, 4);
        if (stream->gcount() >= chunkSize)
        {
            stream->seekg(chunkSize, std::ios_base::cur);
        }
        if (stream->gcount() >= 4)
        {
            fmtChunkId = wav_get_LE(stream, 4);
        }
    }

    if (fmtChunkId != FMT_CHUNK_ID)
    {
        return false;
    }

    auto lengthOfFormatData = wav_get_LE(stream, 4);  // Offset 16
    auto isPCM = wav_get_LE(stream, 2) == 1;          // Offset 20
    numChannels = wav_get_LE(stream, 2);              // Offset 22
    sampleRate = wav_get_LE(stream, 4);               // Offset 24
    /*auto avgBytesPerSecond =*/ wav_get_LE(stream, 4);   // Offset 28
    /*auto blockAlign =*/ wav_get_LE(stream, 2);          // Offset 32
    validBits = wav_get_LE(stream, 2);                // Offset 34
    
    if (lengthOfFormatData != 16) {
        stream->ignore(lengthOfFormatData - 16);
    }
    
    auto dataChunkId = wav_get_LE(stream, 4);         // Ofset 36

    // Skip fact and smpl chunks
    const int maxRetries = 10;
    int retryCounter = 0;

    while (dataChunkId != DATA_CHUNK_ID && retryCounter++ != maxRetries)
    {
        auto factOrOtherChunkSize = wav_get_LE(stream, 4);
        stream->ignore(factOrOtherChunkSize);
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

    if (sampleRate < 11025 || sampleRate > 96000)
    {
        return false;
    }

    if (validBits != 16 && validBits != 24 && validBits != 32)
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
