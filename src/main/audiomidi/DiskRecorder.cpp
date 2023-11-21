#include "DiskRecorder.hpp"

#include "Mpc.hpp"

#include "WavOutputFileStream.hpp"
#include "AudioMidiServices.hpp"

#include "engine/audio/core/AudioFormat.hpp"
#include "engine/audio/core/AudioBuffer.hpp"

using namespace mpc::audiomidi;
using namespace mpc::engine::audio::core;

const std::vector<std::pair<std::string, std::string>> DiskRecorder::fileNamesMono{ {"L.wav", "R.wav"}, {"1.wav", "2.wav"}, {"3.wav", "4.wav"}, {"5.wav", "6.wav"}, {"7.wav", "8.wav"}};
const std::vector<std::string> DiskRecorder::fileNamesStereo{"L-R.wav", "1-2.wav", "3-4.wav", "5-6.wav", "7-8.wav" };

DiskRecorder::DiskRecorder(mpc::Mpc& mpcToUse, AudioProcess* process, int indexToUse)
	: AudioProcessAdapter(process), mpc(mpcToUse), index(indexToUse)
{
}

bool DiskRecorder::prepare(int lengthInFramesToUse, int sampleRate, bool isStereo)
{
    if (bufferLeft.empty())
    {
        ringBufferLeft = moodycamel::ReaderWriterQueue<float>(BUFFER_SIZE);
        ringBufferRight = moodycamel::ReaderWriterQueue<float>(BUFFER_SIZE);
        bufferLeft = std::vector<float>(BUFFER_SIZE);
        bufferRight = std::vector<float>(BUFFER_SIZE);
    }

    if (writing.load())
    {
        return false;
    }

	lengthInFrames = lengthInFramesToUse;

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
        byteBufferLeft.clear();
        byteBufferRight.clear();
        stereoByteBuffer = std::vector<char>(BUFFER_SIZE * 2 * 2);
    }
    else
    {
        byteBufferLeft = std::vector<char>(BUFFER_SIZE * 2);
        byteBufferRight = std::vector<char>(BUFFER_SIZE * 2);
        stereoByteBuffer.clear();
    }
	
    delete outputFileFormat;

    outputFileFormat = new AudioFormat(sampleRate, 16, (isStereo ? 2 : 1), true, false);

    preparedToWrite.store(true);

    writeThread = std::thread([this]{
        while (preparedToWrite.load() || writing.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            writeRingBufferToDisk();
        }

        ringBufferLeft = moodycamel::ReaderWriterQueue<float>(0);
        ringBufferRight = moodycamel::ReaderWriterQueue<float>(0);
        bufferLeft.clear();
        bufferRight.clear();

        byteBufferLeft.clear();
        byteBufferRight.clear();
        stereoByteBuffer.clear();
    });

    if (writeThread.joinable())
    {
        writeThread.detach();
    }

    return true;
}

int DiskRecorder::processAudio(AudioBuffer* buf, int nFrames)
{
	const auto ret = AudioProcessAdapter::processAudio(buf, nFrames);

	if (writing.load())
	{
        const auto sourceBufferLeft = buf->getChannel(0);
        const auto sourceBufferRight = buf->getChannel(1);

        int bufferOffset = 0;

        if (writtenByteCount == 0)
        {
            bufferOffset = mpc.getAudioMidiServices()->getFrameSequencer()->getBounceFrameOffset();
        }

        for (int f = bufferOffset; f < nFrames; f++)
        {
            ringBufferLeft.enqueue(sourceBufferLeft[f]);
            ringBufferRight.enqueue(sourceBufferRight[f]);
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

void DiskRecorder::writeRingBufferToDisk()
{
    const auto availableFrames =
            std::min<size_t>(ringBufferLeft.size_approx(), ringBufferRight.size_approx());

    if (availableFrames == 0)
    {
        return;
    }

    auto bytesToWritePerChannel = availableFrames * 2;

    for (int frame = 0; frame < availableFrames; frame++)
    {
        bufferLeft[frame] = *ringBufferLeft.peek();
        bufferRight[frame] = *ringBufferRight.peek();
        ringBufferLeft.pop();
        ringBufferRight.pop();
    }

    if (outputFileFormat->getChannels() == 1)
    {
        FloatSampleTools::float2byteGeneric(bufferLeft, 0, byteBufferLeft, 0, outputFileFormat->getFrameSize(),
                                            availableFrames, outputFileFormat, 0.f);
        FloatSampleTools::float2byteGeneric(bufferRight, 0, byteBufferRight, 0, outputFileFormat->getFrameSize(),
                                            availableFrames, outputFileFormat, 0.f);
    }
    else if (outputFileFormat->getChannels() == 2)
    {
        FloatSampleTools::float2byteGeneric(bufferLeft, 0, stereoByteBuffer, 0, outputFileFormat->getFrameSize(),
                                            availableFrames, outputFileFormat, 0.f);
        FloatSampleTools::float2byteGeneric(bufferRight, 0, stereoByteBuffer, outputFileFormat->getFrameSize() / 2, outputFileFormat->getFrameSize(),
                                            availableFrames, outputFileFormat, 0.f);
    }

    if (outputFileFormat->getChannels() == 1 && bytesToWritePerChannel + writtenByteCount >= lengthInBytes)
    {
        bytesToWritePerChannel = lengthInBytes - writtenByteCount;
        writing.store(false);
    }
    else if (outputFileFormat->getChannels() == 2 && (bytesToWritePerChannel * 2) + writtenByteCount >= lengthInBytes)
    {
        bytesToWritePerChannel = (lengthInBytes - writtenByteCount) / 2;
        writing.store(false);
    }

    if (outputFileFormat->getChannels() == 1)
    {
        wav_write_bytes(fileStreams[0], byteBufferLeft, bytesToWritePerChannel);
        wav_write_bytes(fileStreams[1], byteBufferRight, bytesToWritePerChannel);
    }
    else if (outputFileFormat->getChannels() == 2)
    {
        wav_write_bytes(fileStreams[0], stereoByteBuffer, bytesToWritePerChannel * 2);
    }

    if (writtenByteCount == 0)
    {
        preparedToWrite.store(false);
    }

    writtenByteCount += bytesToWritePerChannel;

    if (outputFileFormat->getChannels() == 2)
    {
        writtenByteCount += bytesToWritePerChannel;
    }

    if (!writing.load() && fileStreams[0].is_open())
    {
        for (auto& fileStream : fileStreams)
        {
            wav_close(fileStream, lengthInFrames, outputFileFormat->getChannels());
        }

        fileStreams.clear();

        lengthInBytes = 0;
        lengthInFrames = 0;
    }
}

bool DiskRecorder::stopEarly()
{
    if (!writing.load())
    {
        return false;
    }

	writing.store(false);
    preparedToWrite.store(false);

    auto bytesPerFrame = outputFileFormat->getChannels() == 1 ? 2 : 4;

	auto writtenFrames = writtenByteCount / bytesPerFrame;

    for (auto& fileStream : fileStreams)
    {
        wav_close(fileStream, writtenFrames, outputFileFormat->getChannels());
    }

    fileStreams.clear();

	writtenByteCount = 0;
	lengthInBytes = 0;
	lengthInFrames = 0;

	delete outputFileFormat;
    outputFileFormat = nullptr;

	return true;
}

DiskRecorder::~DiskRecorder()
{
    if (writing.load())
    {
        stopEarly();
    }

    if (writeThread.joinable())
    {
        writeThread.join();
    }

    fileStreams.clear();
    delete outputFileFormat;
}
