#include <audiomidi/DiskRecorder.hpp>

#include <audio/core/AudioFormat.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <audiomidi/WavOutputFileStream.hpp>

using namespace std;
using namespace mpc::audiomidi;

DiskRecorder::DiskRecorder(ctoot::audio::core::AudioProcess* process, string name)
	: AudioProcessAdapter(process)
{
	this->name = name;
}

bool DiskRecorder::prepare(const std::string& absolutePath, int lengthInFrames, int sampleRate)
{
	if (writing)
		return false;

	this->lengthInFrames = lengthInFrames;
	this->sampleRate = sampleRate;

	if (fileStream.is_open())
		fileStream.close();

	fileStream = wav_init_ofstream(absolutePath);
	wav_writeHeader(fileStream, sampleRate);
	
	lengthInBytes = lengthInFrames * 2 * 2; // assume 16 bit stereo for now
	
	if (format != nullptr)
		delete format;

	format = new ctoot::audio::core::AudioFormat(sampleRate, 16, 2, true, false);
	
	return true;
}

int DiskRecorder::processAudio(ctoot::audio::core::AudioBuffer* buf)
{
	auto ret = AudioProcessAdapter::processAudio(buf);
	
	if (writing)
	{
		vector<char> audioBufferAsBytes (buf->getByteArrayBufferSize(format, buf->getSampleCount()));
		buf->convertToByteArray_(0, buf->getSampleCount(), &audioBufferAsBytes, 0, format);
		
		if (audioBufferAsBytes.size() + written >= lengthInBytes)
		{
			audioBufferAsBytes.resize(lengthInBytes - written);
			writing = false;
		}
		
		wav_write_bytes(fileStream, audioBufferAsBytes);
		written += audioBufferAsBytes.size();

		if (!writing && fileStream.is_open())
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

	written = 0;
	writing = true;

	return true;
}

bool DiskRecorder::stop()
{
	if (!fileStream.is_open() || !writing)
		return false;

	writing = false;
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
