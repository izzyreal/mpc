#include <audiomidi/DiskRecorder.hpp>

#include <engine/audio/core/AudioFormat.hpp>
#include <engine/audio/core/AudioBuffer.hpp>

#include <audiomidi/WavOutputFileStream.hpp>

#include <Logger.hpp>
#include <utility>

using namespace mpc::audiomidi;
using namespace mpc::engine::audio::core;

DiskRecorder::DiskRecorder(AudioProcess* process, std::string nameToUse)
	: AudioProcessAdapter(process), name(std::move(nameToUse))
{
}

bool DiskRecorder::prepare(const std::string& absolutePath, int lengthInFramesToUse, int sampleRateToUse)
{
	if (writing.load())
		return false;

	this->lengthInFrames = lengthInFramesToUse;
	this->sampleRate = sampleRateToUse;

	if (fileStream.is_open())
		fileStream.close();

	fileStream = wav_init_ofstream(absolutePath);

	if (!fileStream.is_open())
		return false;

	wav_writeHeader(fileStream, sampleRateToUse);

	lengthInBytes = lengthInFramesToUse * 2 * 2; // assume 16 bit stereo for now

	if (format != nullptr)
		delete format;

	format = new AudioFormat(sampleRateToUse, 16, 2, true, false);

	return true;
}

int DiskRecorder::processAudio(AudioBuffer* buf, int nFrames)
{
	auto ret = AudioProcessAdapter::processAudio(buf, nFrames);

	if (writing.load())
	{
        std::vector<char> audioBufferAsBytes (FloatSampleBuffer::getByteArrayBufferSize(format, nFrames));
		buf->convertToByteArray_(0, nFrames, audioBufferAsBytes, 0, format);
		
		if (audioBufferAsBytes.size() + writtenByteCount >= lengthInBytes)
		{
			audioBufferAsBytes.resize(lengthInBytes - writtenByteCount);
			writing.store(false);
		}
		
		wav_write_bytes(fileStream, audioBufferAsBytes);
		writtenByteCount += audioBufferAsBytes.size();

		if (!writing.load() && fileStream.is_open())
		{
			wav_close(fileStream, sampleRate, lengthInFrames);
			lengthInBytes = 0;
			lengthInFrames = 0;
			sampleRate = 0;

			if (format != nullptr)
			{
				delete format;
				format = nullptr;
			}
		}
	}

	return ret;
}

bool DiskRecorder::start()
{
	if (!fileStream.is_open())
		return false;

	writtenByteCount = 0;
	writing.store(true);

	return true;
}

bool DiskRecorder::stopEarly()
{
	if (!fileStream.is_open() || !writing.load())
		return false;
	
	writing.store(false);

	auto writtenFrames = writtenByteCount / 4;

	wav_close(fileStream, sampleRate, writtenFrames);

	writtenByteCount = 0;
	lengthInBytes = 0;
	lengthInFrames = 0;
	sampleRate = 0;

	if (format != nullptr)
	{
		delete format;
		format = nullptr;
	}

	return true;
}

DiskRecorder::~DiskRecorder()
{
	if (fileStream.is_open())
		fileStream.close();

	if (format != nullptr)
		delete format;
}
