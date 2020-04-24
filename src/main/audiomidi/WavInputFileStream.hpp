#pragma once
#include <fstream>
#include <vector>

using namespace std;

static const int RIFF_CHUNK_ID{ 1179011410 };
static const int RIFF_TYPE_ID{ 1163280727 };
static const int FMT_CHUNK_ID{ 544501094 };
static const int DATA_CHUNK_ID{ 1635017060 };

static const int EXPECTED_HEADER_SIZE = 44;
static const int EXPECTED_FMT_DATA_SIZE = 16;

ifstream wav_init_ifstream(const string& path) {
    ifstream result(path.c_str(), ios::in | ios::binary);
    result.unsetf(ios_base::skipws);
	return result;
}

int byteArrayToLE(char* bytes) {
   int value = 0;
   for (unsigned int i = 0; i < sizeof(int); ++i) {
       value |= *(bytes + i) << 8 * i;
   }
   return value;
}

int wav_getLE(ifstream& stream, int numBytes)
{

    if (numBytes < 1 || numBytes > 4) {
        return 0;
    }

    int pos = 0;
    char buffer[4];

    for (int i = 0; i < numBytes; i++) {
        char c;
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

bool wav_readHeader(ifstream& stream, int& sampleRate, int& validBits, int& numChannels, int& dataChunkSize, int& numFrames) {
    
    stream.seekg(0, stream.end);
    auto tell = stream.tellg();

    if (tell < EXPECTED_HEADER_SIZE) {
        return false;
    }
    stream.seekg(0, stream.beg);

    auto riffChunkId = wav_getLE(stream, 4);         // Offset 0
    auto mainChunkSize = wav_getLE(stream, 4);       // Offset 4;
    auto riffTypeId = wav_getLE(stream, 4);          // Offset 8
    auto fmtChunkId = wav_getLE(stream, 4);          // Offset 12
    auto lengthOfFormatData = wav_getLE(stream, 4);  // Offset 16
    auto isPCM = wav_getLE(stream, 2) == 1;          // Offset 20
    numChannels = wav_getLE(stream, 2);         // Offset 22
    sampleRate = wav_getLE(stream, 4);               // Offset 24
    auto avgBytesPerSecond = wav_getLE(stream, 4);   // Offset 28
    auto blockAlign = wav_getLE(stream, 2);          // Offset 32
    validBits = wav_getLE(stream, 2);           // Offset 34
    auto dataChunkId = wav_getLE(stream, 4);         // Ofset 36
    dataChunkSize = wav_getLE(stream, 4);       // Offset 40

    if (riffChunkId != RIFF_CHUNK_ID) {
        return false;
    }

    if (fmtChunkId != FMT_CHUNK_ID) {
        return false;
    }

    if (lengthOfFormatData != EXPECTED_FMT_DATA_SIZE) {
        return false;
    }

    if (!isPCM) {
        return false;
    }

    if (numChannels != 1 && numChannels != 2) {
        return false;
    }

    if (sampleRate < 11025 || sampleRate > 48000) {
        return false;
    }

    if (validBits != 16) {
        return false;
    }

    if (mainChunkSize + 8 != tell) {
        return false;
    }

    numFrames = (dataChunkSize / (validBits / 8)) / numChannels;

    return true;
}

void wav_read_bytes(ifstream& stream, const vector<char>& bytes) {
   stream.read((char*)(&bytes[0]), bytes.size());
}

void wav_close(ifstream& stream) {
    stream.close();
}