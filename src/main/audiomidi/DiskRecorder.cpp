#include <audiomidi/DiskRecorder.hpp>

#include <engine/audio/core/AudioFormat.hpp>
#include <engine/audio/core/AudioBuffer.hpp>

#include <audiomidi/WavOutputFileStream.hpp>
#include "Mpc.hpp"

using namespace mpc::audiomidi;
using namespace mpc::engine::audio::core;

const std::vector<std::pair<std::string, std::string>> DiskRecorder::fileNamesMono{ {"L.wav", "R.wav"}, {"1.wav", "2.wav"}, {"3.wav", "4.wav"}, {"5.wav", "6.wav"}, {"7.wav", "8.wav"}};
const std::vector<std::string> DiskRecorder::fileNamesStereo{"L-R.wav", "1-2.wav", "3-4.wav", "5-6.wav", "7-8.wav" };

DiskRecorder::DiskRecorder(mpc::engine::audio::core::AudioProcess* process, int indexToUse)
	: AudioProcessAdapter(process), index(indexToUse)
{
}

bool DiskRecorder::prepare(mpc::Mpc& mpc, int lengthInFrames, int sampleRate, bool isStereo)
{
    if (writing.load())
    {
        return false;
    }

	this->lengthInFrames = lengthInFrames;

    for (int i = 0; i < (isStereo ? 1 : 2); i++)
    {
        const auto fileName = isStereo ? fileNamesStereo[index] :
                (i == 0 ? fileNamesMono[index].first : fileNamesMono[index].second);

        auto absolutePath = mpc.paths->recordingsPath() / fileName;

        fileStreams.push_back(wav_init_ofstream(absolutePath));

        if (!fileStreams.back().is_open())
        {
            fileStreams.clear();
            return false;
        }
    }

    for (auto& fileStream : fileStreams)
    {
        wav_writeHeader(fileStream, sampleRate, isStereo ? 2 : 1);
    }

    const int numBytesPerSample = 2; // assume 16 bit PCM

	lengthInBytes = lengthInFrames * numBytesPerSample;

    if (isStereo)
    {
        lengthInBytes *= 2;
    }
	
	if (format != nullptr)
    {
        delete format;
    }

	format = new mpc::engine::audio::core::AudioFormat(sampleRate, 16, (isStereo ? 2 : 1), true, false);
	
	return true;
}

int DiskRecorder::processAudio(AudioBuffer* buf, int nFrames)
{
	auto ret = AudioProcessAdapter::processAudio(buf, nFrames);
	
	if (writing.load())
	{
        if (format->getChannels() == 1)
        {
            AudioBuffer bufLeft("", 2, nFrames, buf->getSampleRate());
            bufLeft.copyFrom(buf);
            bufLeft.removeChannel(1);

            AudioBuffer bufRight("", 2, nFrames, buf->getSampleRate());
            bufRight.copyFrom(buf);
            bufRight.removeChannel(0);

            std::vector<char> audioBufferAsBytesLeft(bufLeft.getByteArrayBufferSize(format, nFrames));
            bufLeft.convertToByteArray_(0, nFrames, audioBufferAsBytesLeft, 0, format);

            std::vector<char> audioBufferAsBytesRight(bufRight.getByteArrayBufferSize(format, nFrames));
            bufRight.convertToByteArray_(0, nFrames, audioBufferAsBytesRight, 0, format);

            if (audioBufferAsBytesLeft.size() + writtenByteCount >= lengthInBytes)
            {
                audioBufferAsBytesLeft.resize(lengthInBytes - writtenByteCount);
                audioBufferAsBytesRight.resize(lengthInBytes - writtenByteCount);

                writing.store(false);
            }

            wav_write_bytes(fileStreams[0], audioBufferAsBytesLeft);
            wav_write_bytes(fileStreams[1], audioBufferAsBytesRight);

            writtenByteCount += audioBufferAsBytesLeft.size();

            if (!writing.load() && fileStreams[0].is_open())
            {
                wav_close(fileStreams[0], lengthInFrames);
                wav_close(fileStreams[1], lengthInFrames);
                lengthInBytes = 0;
                lengthInFrames = 0;

                if (format != nullptr)
                {
                    delete format;
                    format = nullptr;
                }

                fileStreams.clear();
            }
        }
        else if (format->getChannels() == 2)
        {
            std::vector<char> audioBufferAsBytes (buf->getByteArrayBufferSize(format, nFrames));
            buf->convertToByteArray_(0, nFrames, audioBufferAsBytes, 0, format);

            if (audioBufferAsBytes.size() + writtenByteCount >= lengthInBytes)
            {
                audioBufferAsBytes.resize(lengthInBytes - writtenByteCount);
                writing.store(false);
            }

            wav_write_bytes(fileStreams[0], audioBufferAsBytes);
            writtenByteCount += audioBufferAsBytes.size();

            if (!writing.load() && fileStreams[0].is_open())
            {
                wav_close(fileStreams[0], lengthInFrames);
                lengthInBytes = 0;
                lengthInFrames = 0;

                if (format != nullptr)
                {
                    delete format;
                    format = nullptr;
                }

                fileStreams.clear();
            }
        }
	}

	return ret;
}

bool DiskRecorder::start()
{
	for (auto& fileStream : fileStreams)
    {
        if (!fileStream.is_open())
        {
            return false;
        }
    }

	writtenByteCount = 0;
	writing.store(true);

	return true;
}

bool DiskRecorder::stopEarly()
{
    if (!writing.load())
    {
        return false;
    }

    for (auto& fileStream : fileStreams)
    {
        if (!fileStream.is_open())
        {
            return false;
        }
    }

	writing.store(false);

    auto bytesPerFrame = format->getChannels() == 1 ? 2 : 4;

	auto writtenFrames = writtenByteCount / bytesPerFrame;

    for (auto& fileStream : fileStreams)
    {
        wav_close(fileStream, writtenFrames);
    }

	writtenByteCount = 0;
	lengthInBytes = 0;
	lengthInFrames = 0;

	if (format != nullptr)
	{
		delete format;
		format = nullptr;
	}

	return true;
}

DiskRecorder::~DiskRecorder()
{
    fileStreams.clear();

	if (format != nullptr)
    {
        delete format;
    }
}
