#include <audiomidi/DiskRecorder.hpp>

#include <audio/core/AudioFormat.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <audiomidi/WavOutputFileStream.hpp>

#include <Logger.hpp>

using namespace std;
using namespace mpc::audiomidi;

DiskRecorder::DiskRecorder(ctoot::audio::core::AudioProcess* process, string name)
	: AudioProcessAdapter(process)
{
	this->name = name;
}

bool DiskRecorder::prepare(const std::string& absolutePath, int lengthInFrames, int sampleRate)
{
	if (writing.load())
		return false;

	this->lengthInFrames = lengthInFrames;
	this->sampleRate = sampleRate;

	if (fileStream.is_open())
		fileStream.close();

	fileStream = wav_init_ofstream(absolutePath);

	if (!fileStream.is_open())
		return false;

	wav_writeHeader(fileStream, sampleRate);
	
	lengthInBytes = lengthInFrames * 2 * 2; // assume 16 bit stereo for now
	
	if (format != nullptr)
		delete format;

	format = new ctoot::audio::core::AudioFormat(sampleRate, 16, 2, true, false);
	
	return true;
}

int DiskRecorder::processAudio(ctoot::audio::core::AudioBuffer* buf, int nFrames)
{
	auto ret = AudioProcessAdapter::processAudio(buf, nFrames);
	
	if (writing.load())
	{
        vector<char> audioBufferAsBytes (buf->getByteArrayBufferSize(format, nFrames));
		buf->convertToByteArray_(0, nFrames, &audioBufferAsBytes, 0, format);
		
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
