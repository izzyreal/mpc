#include <audiomidi/SoundPlayer.hpp>

#include <audio/core/AudioFormat.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <cmath>

#include "WavInputFileStream.hpp"
#include "SndInputFileStream.hpp"

using namespace std;
using namespace mpc::sampler;
using namespace mpc::audiomidi;

SoundPlayer::SoundPlayer()
{
}

// Should be called from the audio thread
void SoundPlayer::start(const string& filePath) {

	if (playing.load()) {
		return;
	}

	auto dot = filePath.find_last_of('.');

	if (dot == string::npos) {
		return;
	}

	auto extension = filePath.substr(dot + 1);
	transform(begin(extension), end(extension), begin(extension), [](char& c) { return tolower(c); });

	isWav = extension.compare("wav") == 0;
	isSnd = extension.compare("snd") == 0;

	if (!isWav && !isSnd) {
		return;
	}

	int validBits;
	int sourceSampleRate;
	int sourceNumChannels;

	bool valid = false;

	unique_lock<mutex> guard(_playing);

	if (isWav) {
		stream = wav_init_ifstream(filePath);
		valid = wav_read_header(stream, sourceSampleRate, validBits, sourceNumChannels, sourceFrameCount);
		}
	else {
		stream = snd_init_ifstream(filePath);
		valid = snd_read_header(stream, sourceSampleRate, validBits, sourceNumChannels, sourceFrameCount);
	}

	if (valid) {
		sourceFormat = make_shared<AudioFormat>(sourceSampleRate, validBits, sourceNumChannels, true, false);
		playing.store(true);
	}
	else {
		stream.close();
	}

}

bool SoundPlayer::isPlaying() {
	return playing.load();
}

void SoundPlayer::stop() {

	if (!playing.load()) {
		return;
	}

	unique_lock<mutex> guard(_playing);

	playing.store(false);

	if (stream.is_open()) {
		stream.close();
	}

	if (srcLeft != nullptr) {
		src_delete(srcLeft);
		srcLeft = nullptr;
	}

	if (srcRight != nullptr) {
		src_delete(srcRight);
		srcRight = nullptr;
	}

	playedSourceFrameCount = 0;

	resampleInputBufferLeft.reset();
	resampleInputBufferRight.reset();
	resampleOutputBufferLeft.reset();
	resampleOutputBufferRight.reset();

}

int SoundPlayer::processAudio(AudioBuffer* buf)
{
	auto left = buf->getChannel(0);
	auto right = buf->getChannel(1);

	if (!playing.load()) {
		buf->makeSilence();
		return AUDIO_SILENCE;
	}

	auto resample = buf->getSampleRate() != sourceFormat->getSampleRate();

	unique_lock<mutex> guard(_playing);

	if (resample) {
		if (srcLeft == nullptr || (sourceFormat->getChannels() >= 2 && srcRight == nullptr)) {
			initSrc();
		}
	}

	bool shouldClose = false;
	auto frameCountToRead = (int)ceil((sourceFormat->getSampleRate() / buf->getSampleRate()) * buf->getSampleCount());

	if (playedSourceFrameCount + frameCountToRead >= sourceFrameCount) {
		shouldClose = true;
		frameCountToRead = sourceFrameCount - playedSourceFrameCount;
	}

	auto sourceBuffer = make_shared<AudioBuffer>("temp", sourceFormat->getChannels(), frameCountToRead, sourceFormat->getSampleRate());
	vector<char> byteBuffer(sourceBuffer->getByteArrayBufferSize(sourceFormat.get()));

	if (isWav || (isSnd && sourceFormat->getChannels() == 1)) {
		stream.read(&byteBuffer[0], byteBuffer.size());
	}
	else {
		auto bytesPerChannel = byteBuffer.size() * 0.5;
		
		vector<char> byteBufferLeft(bytesPerChannel);
		vector<char> byteBufferRight(bytesPerChannel);
		
		stream.read(&byteBufferLeft[0], bytesPerChannel);
		stream.seekg(byteBuffer.size(), ios_base::cur);
		stream.seekg(-bytesPerChannel, ios_base::cur);
		stream.read(&byteBufferRight[0], bytesPerChannel);
		stream.seekg(-byteBuffer.size(), ios_base::cur);
		
		int byteCounter = 0;

		for (int i = 0; i < byteBuffer.size(); i += 4) {
			byteBuffer[i] = byteBufferLeft[byteCounter];
			byteBuffer[i + 1] = byteBufferLeft[byteCounter + 1];
			byteBuffer[i + 2] = byteBufferRight[byteCounter];
			byteBuffer[i + 3] = byteBufferRight[byteCounter + 1];
			byteCounter += 2;
		}
	}
	
	sourceBuffer->initFromByteArray_(byteBuffer, 0, byteBuffer.size(), sourceFormat.get());

	if (shouldClose) {
		buf->makeSilence();
	}

	if (resample) {

		if (sourceFormat->getChannels() >= 1) {
			resampleChannel(true, sourceBuffer->getChannel(0), sourceFormat->getSampleRate(), buf->getSampleRate(), false);
		}

		if (sourceFormat->getChannels() >= 2) {
			resampleChannel(false, sourceBuffer->getChannel(1), sourceFormat->getSampleRate(), buf->getSampleRate(), false);
		}

		if (resampleOutputBufferLeft.available() >= buf->getSampleCount()) {

			for (int i = 0; i < buf->getSampleCount(); i++) {
				(*left)[i] = resampleOutputBufferLeft.get();
			}

			if (sourceFormat->getChannels() == 1) {
				buf->copyChannel(0, 1);
			}
			else {
				for (int i = 0; i < buf->getSampleCount(); i++) {
					(*right)[i] = resampleOutputBufferRight.get();
				}
			}
		}
		else if (shouldClose && resampleOutputBufferLeft.available() > 0) {

			auto remaining = resampleOutputBufferLeft.available();

			for (int i = 0; i < remaining; i++) {
				(*left)[i] = resampleOutputBufferLeft.get();
			}

			if (sourceFormat->getChannels() == 1) {
				buf->copyChannel(0, 1);
			}
			else {
				for (int i = 0; i < remaining; i++) {
					(*right)[i] = resampleOutputBufferRight.get();
				}
			}
		}
	}
	else {

		if (sourceBuffer->getSampleCount() < buf->getSampleCount()) {
			sourceBuffer->changeSampleCount(buf->getSampleCount(), true);
		}

		if (sourceBuffer->getChannelCount() == 1) {
			
			for (int i = 0; i < buf->getSampleCount(); i++) {
				(*left)[i] = (*sourceBuffer->getChannel(0))[i];
				(*right)[i] = (*left)[i];
			}
		}
		else {
			for (int i = 0; i < buf->getSampleCount(); i++) {
				(*left)[i] = (*sourceBuffer->getChannel(0))[i];
				(*right)[i] = (*sourceBuffer->getChannel(1))[i];
			}
		}
	}

	playedSourceFrameCount += frameCountToRead;

	
	guard.unlock();

	if (shouldClose) {
		stop();
	}

	return AUDIO_OK;
}

void SoundPlayer::resampleChannel(bool left, vector<float>* inputBuffer, int sourceSampleRate, int destinationSampleRate, bool endOfInput)
{
	auto ratio = static_cast<float>(destinationSampleRate) / static_cast<float>(sourceSampleRate);
	auto circularInputBuffer = left ? &resampleInputBufferLeft : &resampleInputBufferRight;
	auto circularOutputBuffer = left ? &resampleOutputBufferLeft : &resampleOutputBufferRight;

	//MLOG("circularInputBuffer size before inputBuffer insertion: " + to_string(circularInputBuffer->available()));

	for (auto f : (*inputBuffer)) {
		circularInputBuffer->put(f);
	}

	//MLOG("inputBuffer size: " + to_string(inputBuffer->size()));
	//MLOG("circularInputBuffer size after inputBuffer insertion: " + to_string(circularInputBuffer->available()));

	vector<float> input;
	while (!circularInputBuffer->empty()) {
		input.push_back(circularInputBuffer->get());
	}

	auto output = vector<float>(ceil(input.size() * ratio));

	//MLOG("input size: " + to_string(input.size()));
	//MLOG("output size: " + to_string(output.size()));

	SRC_DATA data;
	data.data_in = &input[0];
	data.input_frames = input.size();
	data.data_out = &output[0];
	data.output_frames = output.size();
	data.end_of_input = 0;
	data.src_ratio = ratio;

	src_process(left ? srcLeft : srcRight, &data);

	//MLOG("input frames used: " + to_string(data.input_frames_used) + ", output frames generated: " + to_string(data.output_frames_gen));

	circularInputBuffer->move(-(input.size() - data.input_frames_used));

	for (int i = 0; i < data.output_frames_gen; i++) {
		circularOutputBuffer->put(output[i]);
	}
}

void SoundPlayer::initSrc() {
	MLOG("initSrc");
	if (srcLeft == nullptr && sourceFormat->getChannels() >= 1) {
		srcLeft = src_new(0, 1, &srcLeftError);
	}

	if (srcRight == nullptr && sourceFormat->getChannels() >= 2) {
		srcRight = src_new(0, 1, &srcRightError);
	}
}

SoundPlayer::~SoundPlayer() {
}
