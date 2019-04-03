#include <audiomidi/ExportAudioProcessAdapter.hpp>

//#include <Util.hpp>
//#include <disk/MpcFile.hpp>
#include <file/wav/WavFile.hpp>
#include <StartUp.hpp>
#include <audio/core/AudioFormat.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <file/File.hpp>
#include <file/FileUtil.hpp>
#include <io/FileOutputStream.hpp>
#include <file/ByteUtil.hpp>

#include <Logger.hpp>

#include <chrono>
#include <thread>

#ifdef __linux__
#include <pthread.h>
#endif // __linux__

using namespace mpc::audiomidi;
using namespace std;
using namespace moduru::io;
using namespace moduru::file;

ExportAudioProcessAdapter::ExportAudioProcessAdapter(ctoot::audio::core::AudioProcess* process, weak_ptr<ctoot::audio::core::AudioFormat> format, string name)
	: AudioProcessAdapter(process)
{
	reading = false;
	writing = false;
	this->format = format;
	this->name = name;
}

int ExportAudioProcessAdapter::processAudio(ctoot::audio::core::AudioBuffer* buf)
{
	auto ret = super::processAudio(buf);
	if (reading) {
		auto lFormat = format.lock();
		vector<char> ba(buf->getByteArrayBufferSize(lFormat.get(), buf->getSampleCount()));
		buf->convertToByteArray_(0, buf->getSampleCount(), &ba, 0, lFormat.get());
		for (auto& b : ba)
			circularBuffer->put(b);
	}
	return ret;
}

void ExportAudioProcessAdapter::static_startWriting(void * args)
{
	static_cast<ExportAudioProcessAdapter*>(args)->startWriting();
}

void ExportAudioProcessAdapter::prepare(moduru::file::File* file, int lengthInFrames, int sampleRate)
{
	this->sampleRate = sampleRate;
	if (reading || writing) {
		throw std::invalid_argument("Can't setFile() when already exporting");
	}
	this->file = file;
	//MLOG("Preparing eapa " + file->getName());
	circularBuffer->reset();
	lengthInBytes = lengthInFrames * 2 * 2; // assume 16 bit stereo for now

	if (file->exists()) file->del();

	file->create();
	moduru::io::FileOutputStream tempFileFos(file);

	int written = 0;
	vector<char> buffer(512);

	for (int i = 0; i < 512; i++) {
		buffer[i] = 0;
	}

	while (lengthInBytes - written > 512) {
		tempFileFos.write(buffer);
		written += 512;
	}
	int remaining = lengthInBytes - written;
	for (int i = 0; i < remaining; i++) {
		tempFileFos.write(0);
	}

	tempFileRaf = fstream(file->getPath().c_str(), ios_base::out | ios_base::in | ios_base::binary);
	tempFileRaf.seekp(0);
	//MLOG("Finished preparing eapa " + file->getName());
}

void ExportAudioProcessAdapter::start()
{
	//MLOG("Start() " + file->getName());
	if (file == nullptr) {
		throw std::invalid_argument("null export file");
	}

	if (reading || writing) return;

	if (writeThread.joinable()) writeThread.join();
	reading = true;
	writeThread = thread(&ExportAudioProcessAdapter::static_startWriting, this);
	//MLOG("writeThread = new thread successful for " + file->getName());
}

void ExportAudioProcessAdapter::stop()
{
	if (!reading) return;
	reading = false;
}

void ExportAudioProcessAdapter::startWriting()
{
	//MLOG("eapa " + file->getName() + " startWriting() thread started");
	//circularBuffer.clear();

	int written = 0;
	writing = true;
	// how to set thread priority to low?
	while (reading || !circularBuffer->empty()) {
		auto close = false;
		vector<char> ba;
		while (!circularBuffer->empty()) {
			ba.push_back(circularBuffer->get());
		}
		if (ba.size() + written >= lengthInBytes) {
			ba.resize(lengthInBytes - written);
			close = true;
		}
		if (ba.size() == 0) {
			this_thread::sleep_for(chrono::milliseconds(2));
			continue;
		}
		tempFileRaf.seekp(written);
		tempFileRaf.write(&ba[0], ba.size());
		tempFileRaf.flush();
		if (close) {
			break;
		}
		written += ba.size();
	}
	writing = false;
	reading = false;
	//MLOG("eapa " + file->getName() + " startWriting() thread stopped");
}

void ExportAudioProcessAdapter::writeWav()
{
	//MLOG("writing wav for " + file->getName());
	auto ints = vector<int>(lengthInBytes / 2);
	int  read = 0;
	int converted = 0;
	auto nonZeroDetected = false;
	while (lengthInBytes - read > 512) {
		tempFileRaf.seekg(read);
		auto ba = vector<char>(512);
		tempFileRaf.read(&ba[0], ba.size());
		auto ba2 = vector<char>(2);
		for (int i = 0; i < 512; i += 2) {
			ba2 = vector<char>{ ba[i], ba[i + 1] };
			short value = moduru::file::ByteUtil::bytes2short(ba2);
			ints[converted++] = value;
			if (value != 0) {
				if (!nonZeroDetected)
					nonZeroDetected = true;
			}
		}
		read += 512;
	}
	auto remain = lengthInBytes - read;
	auto remainder = vector<char>(remain);
	tempFileRaf.seekg(read);
	tempFileRaf.read(&remainder[0], remain);
	tempFileRaf.close();
	//	tempFileFos->close();
	file->del();
	for (int i = 0; i < remain; i += 2) {
		auto ba = vector<char>{ remainder[i], remainder[i + 1] };
		auto value = moduru::file::ByteUtil::bytes2short(ba);
		ints[converted++] = value;
		if (value != 0) {
			if (!nonZeroDetected)
				nonZeroDetected = true;
		}
	}

	if (nonZeroDetected) {
		string sep = FileUtil::getSeparator();
		string wavFileName = StartUp::home + sep + "vMPC" + sep + "recordings" + sep + file->getName() + ".WAV";
		moduru::file::File resultWavFile(wavFileName, nullptr);
		if (resultWavFile.exists()) resultWavFile.del();
		resultWavFile.create();
		auto wavFile = mpc::file::wav::WavFile();
		wavFile.newWavFile(2, ints.size() / 2, 16, sampleRate);
		wavFile.writeFrames(&ints, ints.size() / 2);
		wavFile.close();
		auto wavBytes = wavFile.getResult();
		moduru::io::FileOutputStream wavFos(&resultWavFile);
		wavFos.write(wavBytes);
		wavFos.close();
		//resultWavFile.close();
	}
	//MLOG("finished writing wav for " + file->getName());
}

ExportAudioProcessAdapter::~ExportAudioProcessAdapter() {
	tempFileRaf.close();
	if (file != nullptr) {
		file->close();
		delete file;
	}
	if (writeThread.joinable()) writeThread.join();
	delete circularBuffer;
}
