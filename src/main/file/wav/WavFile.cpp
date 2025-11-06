#include "WavFile.hpp"

#ifdef _MSC_VER
#include <string>
#endif

using namespace mpc::file::wav;
using namespace mpc::disk;

const int WavFile::BUFFER_SIZE;
const int WavFile::FMT_CHUNK_ID;
const int WavFile::DATA_CHUNK_ID;
const int WavFile::RIFF_CHUNK_ID;
const int WavFile::RIFF_TYPE_ID;

int WavFile::getNumChannels() const
{
    return numChannels;
}

int WavFile::getNumSampleLoops() const
{
    return numSampleLoops;
}

SampleLoop &WavFile::getSampleLoop()
{
    return sampleLoop;
}

unsigned long WavFile::getNumFrames() const
{
    return numFrames;
}

int WavFile::getSampleRate() const
{
    return sampleRate;
}

int WavFile::getValidBits() const
{
    return validBits;
}

WavFile WavFile::writeWavStream(const std::shared_ptr<std::ostream> &_ostream,
                                int numChannels, int numFrames, int validBits,
                                int sampleRate)
{
    WavFile result;
    result.oStream = _ostream;
    result.numChannels = numChannels;
    result.numFrames = numFrames;
    result.sampleRate = sampleRate;
    result.bytesPerSample = (validBits + 7) / 8;
    result.blockAlign = result.bytesPerSample * numChannels;
    result.validBits = validBits;
    result.numSampleLoops = 0;

    if (numChannels < 1 || numChannels > 65535)
    {
        throw std::invalid_argument(
            "Illegal number of channels, valid range 1 to 65536");
    }

    if (numFrames < 0)
    {
        throw std::invalid_argument("Number of frames must be positive");
    }

    if (validBits < 2 || validBits > 65535)
    {
        throw std::invalid_argument(
            "Illegal number of valid bits, valid range 2 to 65536");
    }

    if (sampleRate < 0)
    {
        throw std::invalid_argument("Sample rate must be positive");
    }

    auto dataChunkSize = result.blockAlign * numFrames;
    auto mainChunkSize = 4 + 8 + 16 + 8 + dataChunkSize;

    if (dataChunkSize % 2 == 1)
    {
        mainChunkSize += 1;
        result.wordAlignAdjust = true;
    }
    else
    {
        result.wordAlignAdjust = false;
    }

    putLE(RIFF_CHUNK_ID, result.buffer, 0, 4);
    putLE(mainChunkSize, result.buffer, 4, 4);
    putLE(RIFF_TYPE_ID, result.buffer, 8, 4);

    result.oStream->write(&result.buffer[0], 12);

    auto averageBytesPerSecond = sampleRate * result.blockAlign;
    putLE(FMT_CHUNK_ID, result.buffer, 0, 4);
    putLE(16, result.buffer, 4, 4);
    putLE(1, result.buffer, 8, 2);
    putLE(numChannels, result.buffer, 10, 2);
    putLE(sampleRate, result.buffer, 12, 4);
    putLE(averageBytesPerSecond, result.buffer, 16, 4);
    putLE(result.blockAlign, result.buffer, 20, 2);
    putLE(validBits, result.buffer, 22, 2);

    result.oStream->write(&result.buffer[0], 24);

    putLE(DATA_CHUNK_ID, result.buffer, 0, 4);
    putLE(dataChunkSize, result.buffer, 4, 4);

    result.oStream->write(&result.buffer[0], 8);

    if (validBits > 8)
    {
        result.floatOffset = 0;
        result.floatScale = 2147483647 >> (32 - validBits); // long max value
    }
    else
    {
        result.floatOffset = 1;
        result.floatScale = 0.5 * ((1 << validBits) - 1);
    }
    result.bufferPointer = 0;
    result.bytesRead = 0;
    result.frameCounter = 0;
    return result;
}

wav_or_error
WavFile::readWavStream(const std::shared_ptr<std::istream> &_istream)
{
    auto result = std::make_shared<WavFile>();
    result->numSampleLoops = 0;
    result->iStream = _istream;
    result->iStream->seekg(0, std::ios::end);

    auto fileSize = result->iStream->tellg();

    result->iStream->seekg(0, std::ios::beg);

    result->iStream->read(&result->buffer[0], 12);

    auto bytesRead = result->iStream->gcount();

    auto totalBytesRead = bytesRead;

    if (bytesRead != 12)
    {
        return tl::make_unexpected(mpc_io_error_msg{"No WAV header found"});
    }

    auto riffChunkID = getLE(result->buffer, 0, 4);
    auto riffTypeID = getLE(result->buffer, 8, 4);

    if (riffChunkID != RIFF_CHUNK_ID)
    {
        return tl::make_unexpected(mpc_io_error_msg{"Invalid riff chunk ID"});
    }

    if (riffTypeID != RIFF_TYPE_ID)
    {
        return tl::make_unexpected(mpc_io_error_msg{"Invalid riff type ID"});
    }

    auto foundFormat = false;
    auto foundData = false;
    std::ios::pos_type dataPos;

    while (totalBytesRead + 1 < fileSize)
    {

        result->iStream->read(&result->buffer[0], 8);
        bytesRead = result->iStream->gcount();

        totalBytesRead += bytesRead;

        if (bytesRead != 8)
        {
            return tl::make_unexpected(
                mpc_io_error_msg{"Could not read chunk header"});
        }

        auto chunkID = getLE(result->buffer, 0, 4);
        auto chunkSize = getLE(result->buffer, 4, 4);
        auto numChunkBytes = (chunkSize % 2 == 1) ? chunkSize + 1 : chunkSize;

        if (chunkID == FMT_CHUNK_ID)
        {
            foundFormat = true;
            result->iStream->read(&result->buffer[0], 16);

            totalBytesRead += chunkSize;

            auto compressionCode =
                static_cast<int>(getLE(result->buffer, 0, 2));

            if (compressionCode != 1)
            {
                return tl::make_unexpected(
                    mpc_io_error_msg{"Compressed WAV unsupported"});
            }

            result->numChannels = static_cast<int>(getLE(result->buffer, 2, 2));
            result->sampleRate = getLE(result->buffer, 4, 4);
            result->blockAlign = static_cast<int>(getLE(result->buffer, 12, 2));
            result->validBits = static_cast<int>(getLE(result->buffer, 14, 2));

            if (result->numChannels == 0)
            {
                return tl::make_unexpected(
                    mpc_io_error_msg{"Zero channels in WAV header"});
            }

            if (result->blockAlign == 0)
            {
                return tl::make_unexpected(
                    mpc_io_error_msg{"Block align in header is 0"});
            }

            if (result->validBits < 2)
            {
                return tl::make_unexpected(
                    mpc_io_error_msg{"Valid bits in header below 2"});
            }

            if (result->validBits > 64)
            {
                return tl::make_unexpected(
                    mpc_io_error_msg{"Valid bits in header over 64"});
            }

            result->bytesPerSample = (result->validBits + 7) / 8;

            if (result->bytesPerSample * result->numChannels !=
                result->blockAlign)
            {
                return tl::make_unexpected(
                    mpc_io_error_msg{"Bad block align for format"});
            }

            numChunkBytes -= 16;

            if (numChunkBytes > 0)
            {
                result->iStream->ignore(numChunkBytes);
            }
        }
        else if (chunkID == DATA_CHUNK_ID)
        {
            if (!foundFormat)
            {
                return tl::make_unexpected(
                    mpc_io_error_msg{"Data before format chunk"});
            }

            if (chunkSize % result->blockAlign != 0)
            {
                return tl::make_unexpected(
                    mpc_io_error_msg{"Bad data size for block align"});
            }

            result->numFrames = chunkSize / result->blockAlign;
            foundData = true;
            totalBytesRead += chunkSize;
            dataPos = result->iStream->tellg();
            result->iStream->ignore(chunkSize);
        }
        else if (chunkID == SMPL_CHUNK_ID)
        {
            result->iStream->read(&result->buffer[0], chunkSize);

            totalBytesRead += chunkSize;

            result->numSampleLoops =
                static_cast<int>(getLE(result->buffer, NUM_SAMPLE_LOOPS, 4));

            if (result->numSampleLoops > 0)
            {
                // For now we just take the first sample loop
                result->sampleLoop.cuePointId = static_cast<int>(
                    getLE(result->buffer, LIST_OF_SAMPLE_LOOPS_OFFSET, 4));
                result->sampleLoop.type = static_cast<int>(
                    getLE(result->buffer, LIST_OF_SAMPLE_LOOPS_OFFSET + 4, 4));
                result->sampleLoop.start = static_cast<int>(
                    getLE(result->buffer, LIST_OF_SAMPLE_LOOPS_OFFSET + 8, 4));
                result->sampleLoop.end = static_cast<int>(
                    getLE(result->buffer, LIST_OF_SAMPLE_LOOPS_OFFSET + 12, 4));
                result->sampleLoop.fraction = static_cast<int>(
                    getLE(result->buffer, LIST_OF_SAMPLE_LOOPS_OFFSET + 16, 4));
                result->sampleLoop.playCount = static_cast<int>(
                    getLE(result->buffer, LIST_OF_SAMPLE_LOOPS_OFFSET + 20, 4));
            }
        }
        else
        {
            result->iStream->ignore(numChunkBytes);
            totalBytesRead += chunkSize;
        }
    }

    if (!foundData)
    {
        return tl::make_unexpected(
            mpc_io_error_msg{"Did not find a data chunk"});
    }

    if (result->validBits > 8)
    {
        result->floatOffset = 0;
        result->floatScale = 1 << (result->validBits - 1);
    }
    else
    {
        result->floatOffset = -1;
        result->floatScale = 0.5 * ((1 << result->validBits) - 1);
    }

    result->bufferPointer = 0;
    result->bytesRead = 0;
    result->frameCounter = 0;
    result->iStream->seekg(dataPos);
    return result;
}

unsigned long WavFile::getLE(std::vector<char> &buffer, unsigned long pos,
                             int numBytes)
{
    numBytes--;
    pos += numBytes;
    unsigned long val = buffer[pos] & 255;
    for (auto b = 0; b < numBytes; b++)
    {
        val = (val << 8) + (buffer[--pos] & 255);
    }
    return val;
}

void WavFile::putLE(int val, std::vector<char> &buffer, unsigned long pos,
                    int numBytes)
{
    for (auto b = 0; b < numBytes; b++)
    {
        buffer[pos] = static_cast<char>(val & 255);
        val >>= 8;
        pos++;
    }
}

void WavFile::writeSample(int val)
{
    for (auto b = 0; b < bytesPerSample; b++)
    {
        if (bufferPointer == BUFFER_SIZE)
        {
            oStream->write(&buffer[0], BUFFER_SIZE);
            bufferPointer = 0;
        }
        buffer[bufferPointer] = static_cast<int8_t>((val & 255));
        val >>= 8;
        bufferPointer++;
    }
}

int WavFile::readSample()
{
    int val = 0;
    for (auto b = 0; b < bytesPerSample; b++)
    {
        if (bufferPointer == bytesRead)
        {
            iStream->read(&buffer[0], BUFFER_SIZE);
            auto read = iStream->gcount();

            if (read == 0)
            {
                return 0;
            }

            bytesRead = read;
            bufferPointer = 0;
        }
        int v = buffer[bufferPointer];
        if (b < bytesPerSample - 1 || bytesPerSample == 1)
        {
            v &= 255;
        }
        val += v << (b * 8);
        bufferPointer++;
    }

    return val;
}

int WavFile::readFrames(std::vector<float> &sampleBuffer,
                        unsigned long numFramesToRead)
{
    int offset = 0;

    if (sampleBuffer.size() != numFramesToRead * numChannels)
    {
        sampleBuffer.resize(numFramesToRead * numChannels);
    }

    for (auto f = 0; f < numFramesToRead; f++)
    {
        if (frameCounter == numFrames)
        {
            return f;
        }
        for (auto c = 0; c < numChannels; c++)
        {
            auto v = readSample();
            sampleBuffer[offset] =
                floatOffset + static_cast<double>(v) / floatScale;
            offset++;
        }
        frameCounter++;
    }
    return numFramesToRead;
}

int WavFile::writeFrames(const std::vector<float> &sampleBuffer,
                         unsigned long numFramesToWrite)
{
    int offset = 0;
    for (auto f = 0; f < numFramesToWrite; f++)
    {
        if (frameCounter == numFrames)
        {
            return f;
        }

        for (auto c = 0; c < numChannels; c++)
        {
            writeSample(static_cast<int>(floatScale *
                                         (floatOffset + sampleBuffer[offset])));
            offset++;
        }
        frameCounter++;
    }
    return numFramesToWrite;
}

void WavFile::close() const
{
    auto ofStream = std::dynamic_pointer_cast<std::ofstream>(oStream);
    if (ofStream && ofStream->is_open())
    {
        if (bufferPointer > 0)
        {
            oStream->write(&buffer[0], bufferPointer);
        }
        if (wordAlignAdjust)
        {
            char buf[1]{0x00};
            oStream->write(buf, 1);
        }
        ofStream->close();
    }
}
