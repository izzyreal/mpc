#include <audiomidi/ExportAudioProcessAdapter.hpp>

#include <StartUp.hpp>
#include <audio/core/AudioFormat.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <file/File.hpp>
#include <file/FileUtil.hpp>
#include <io/FileOutputStream.hpp>
#include <file/ByteUtil.hpp>

#include <Logger.hpp>

#include <audiomidi/WavStream.h>

#include <chrono>
#include <thread>

#ifdef __linux__
#include <pthread.h>
#endif // __linux__

using namespace std;
using namespace mpc::audiomidi;
using namespace moduru::io;
using namespace moduru::file;

ExportAudioProcessAdapter::ExportAudioProcessAdapter(ctoot::audio::core::AudioProcess* process, string name)
	: AudioProcessAdapter(process)
{
	this->name = name;
}

void ExportAudioProcessAdapter::prepare(const std::string& absolutePath, int lengthInFrames, int sampleRate)
{
	if (writing) {
		throw std::invalid_argument("Can't prepare when already exporting");
	}

	this->lengthInFrames = lengthInFrames;
	this->sampleRate = sampleRate;

	if (fileStream.is_open()) {
		fileStream.close();
	}

	fileStream = ws_init(absolutePath);
	ws_writeHeader(fileStream, sampleRate);
	
	lengthInBytes = lengthInFrames * 2 * 2; // assume 16 bit stereo for now
	
	format = new ctoot::audio::core::AudioFormat(sampleRate, 16, 2, true, false);
}

int ExportAudioProcessAdapter::processAudio(ctoot::audio::core::AudioBuffer* buf)
{
	auto ret = AudioProcessAdapter::processAudio(buf);

	
	if (writing) {
		vector<char> audioBufferAsBytes (buf->getByteArrayBufferSize(format, buf->getSampleCount()));
		buf->convertToByteArray_(0, buf->getSampleCount(), &audioBufferAsBytes, 0, format);
		if (audioBufferAsBytes.size() + written >= lengthInBytes) {
			audioBufferAsBytes.resize(lengthInBytes - written);
			writing = false;
		}
		
		ws_write_bytes(fileStream, audioBufferAsBytes);
		written += audioBufferAsBytes.size();
		
		if (!writing) {
			ws_close(fileStream, sampleRate, lengthInFrames);
			lengthInBytes = 0;
			lengthInFrames = 0;
			sampleRate = 0;
			format = nullptr;
		}
	}

	return ret;
}

void ExportAudioProcessAdapter::start()
{
	if (!fileStream.is_open()) {
		throw std::invalid_argument("file stream is not open");
	}
	written = 0;
	writing = true;
}

ExportAudioProcessAdapter::~ExportAudioProcessAdapter() {
	if (fileStream.is_open()) {
		fileStream.close();
	}
	if (format != nullptr) {
		delete format;
	}
	MLOG("destroyed " + name);
}
