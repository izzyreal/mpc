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
	srcLeft = src_new(0, 1, &srcLeftError);
	srcRight = src_new(0, 1, &srcRightError);
}

bool SoundPlayer::start(const string& filePath) {

	unique_lock<mutex> guard(_playing);

	if (playing) {
		return false;
	}

	auto dot = filePath.find_last_of('.');

	if (dot == string::npos) {
		return false;
	}

	auto extension = filePath.substr(dot + 1);
	transform(begin(extension), end(extension), begin(extension), [](char& c) { return tolower(c); });

	isWav = extension.compare("wav") == 0;
	isSnd = extension.compare("snd") == 0;

	if (!isWav && !isSnd) {
		return false;
	}

	int validBits;
	int sourceSampleRate;
	int sourceNumChannels;

	bool valid = false;

	if (isWav) {
		stream = wav_init_ifstream(filePath);

		if (!stream.is_open()) {
			return false;
		}

		valid = wav_read_header(stream, sourceSampleRate, validBits, sourceNumChannels, sourceFrameCount);
	}
	else {
		stream = snd_init_ifstream(filePath);
		valid = snd_read_header(stream, sourceSampleRate, validBits, sourceNumChannels, sourceFrameCount);
	}

	if (!valid) {
		stream.close();
		return false;
	}

	sourceFormat = make_shared<AudioFormat>(sourceSampleRate, validBits, sourceNumChannels, true, false);

	src_reset(srcLeft);
	src_reset(srcRight);
	
	fadeFactor = -1.0f;
	stopEarly = false;

	playing = true;
	
	return true;
}

void SoundPlayer::enableStopEarly()
{

	unique_lock<mutex> guard(_playing);

	stopEarly = true;
	fadeFactor = 1.0f;
}

void SoundPlayer::stop()
{

	unique_lock<mutex> guard(_playing);

	if (!playing) {
		return;
	}

	if (stream.is_open()) {
		stream.close();
	}

	playing = false;
	
	ingestedSourceFrameCount = 0;

	resampleInputBufferLeft.reset();
	resampleInputBufferRight.reset();
	resampleOutputBufferLeft.reset();
	resampleOutputBufferRight.reset();

}

int SoundPlayer::processAudio(AudioBuffer* buf)
{

	unique_lock<mutex> guard(_playing);

	auto left = buf->getChannel(0);
	auto right = buf->getChannel(1);

	if (!playing) {
		buf->makeSilence();
		return AUDIO_SILENCE;
	}

	auto resample = buf->getSampleRate() != sourceFormat->getSampleRate();
	auto resampleRatio = (int) ceil(sourceFormat->getSampleRate() / buf->getSampleRate());

	auto frameCountToRead = resampleRatio * buf->getSampleCount();
	
	auto shouldStop = false;

	if (ingestedSourceFrameCount + frameCountToRead >= sourceFrameCount) {
		shouldStop = true;
		frameCountToRead = sourceFrameCount - ingestedSourceFrameCount;
	}

	auto sourceBuffer = make_shared<AudioBuffer>("temp", sourceFormat->getChannels(), frameCountToRead, sourceFormat->getSampleRate());
	vector<char> byteBuffer(sourceBuffer->getByteArrayBufferSize(sourceFormat.get()));

	if (isWav || (isSnd && sourceFormat->getChannels() == 1)) {
		stream.read(&byteBuffer[0], byteBuffer.size());
	}
	else {
		auto bytesPerChannel = (int) (byteBuffer.size() * 0.5);
		
		vector<char> byteBufferLeft(bytesPerChannel);
		vector<char> byteBufferRight(bytesPerChannel);
		
		auto totalChannelLengthInBytes = sourceFrameCount * 2;

		stream.read(&byteBufferLeft[0], bytesPerChannel);
		stream.seekg(-bytesPerChannel, ios_base::cur);

		stream.seekg(totalChannelLengthInBytes, ios_base::cur);
		stream.read(&byteBufferRight[0], bytesPerChannel);

		stream.seekg(-totalChannelLengthInBytes, ios_base::cur);
		
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

	if (shouldStop) {
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
		else if (shouldStop && resampleOutputBufferLeft.available() > 0) {

			auto remaining = resampleOutputBufferLeft.available();

			for (int i = 0; i < min( (int) remaining, buf->getSampleCount()); i++) {
				(*left)[i] = resampleOutputBufferLeft.get();
			}

			if (sourceFormat->getChannels() == 1) {
				buf->copyChannel(0, 1);
			}
			else {
				for (int i = 0; i < min((int)remaining, buf->getSampleCount()); i++) {
					(*right)[i] = resampleOutputBufferRight.get();
				}
			}
		}
	}
	else {

		auto frameCountToWrite = min(sourceBuffer->getSampleCount(), buf->getSampleCount());
		if (sourceBuffer->getChannelCount() == 1) {
			
			for (int i = 0; i < frameCountToWrite; i++) {
				(*left)[i] = (*sourceBuffer->getChannel(0))[i];
				(*right)[i] = (*left)[i];
			}
		}
		else {
			for (int i = 0; i < frameCountToWrite; i++) {
				(*left)[i] = (*sourceBuffer->getChannel(0))[i];
				(*right)[i] = (*sourceBuffer->getChannel(1))[i];
			}
		}
	}

	ingestedSourceFrameCount += frameCountToRead;

	if (stopEarly) {
		int bufferIndex = 0;

		while (fadeFactor >= 0.0f && bufferIndex < buf->getSampleCount()) {
			(*left)[bufferIndex] = (*left)[bufferIndex] * fadeFactor;
			(*right)[bufferIndex] = (*right)[bufferIndex] * fadeFactor;
			fadeFactor -= 0.002f;
			bufferIndex++;
		}

		if (bufferIndex != buf->getSampleCount()) {
			for (int i = bufferIndex; i < buf->getSampleCount(); i++) {
				(*left)[i] = 0;
				(*right)[i] = 0;
			}
		}
	}


	if (shouldStop || (stopEarly && fadeFactor < 0) ) {
		guard.unlock();
		stop();
	}
	else {
		guard.unlock();
	}

	return AUDIO_OK;
}

void SoundPlayer::resampleChannel(bool left, vector<float>* inputBuffer, int sourceSampleRate, int destinationSampleRate, bool endOfInput)
{
	auto ratio = static_cast<float>(destinationSampleRate) / static_cast<float>(sourceSampleRate);
	auto circularInputBuffer = left ? &resampleInputBufferLeft : &resampleInputBufferRight;
	auto circularOutputBuffer = left ? &resampleOutputBufferLeft : &resampleOutputBufferRight;

	for (auto f : (*inputBuffer)) {
		circularInputBuffer->put(f);
	}

	vector<float> input;
	while (!circularInputBuffer->empty()) {
		input.push_back(circularInputBuffer->get());
	}

	auto output = vector<float>(ceil(input.size() * ratio));

	SRC_DATA data;
	data.data_in = &input[0];
	data.input_frames = input.size();
	data.data_out = &output[0];
	data.output_frames = output.size();
	data.end_of_input = 0;
	data.src_ratio = ratio;

	src_process(left ? srcLeft : srcRight, &data);

	circularInputBuffer->move(-(input.size() - data.input_frames_used));

	for (int i = 0; i < data.output_frames_gen; i++) {
		circularOutputBuffer->put(output[i]);
	}
}

SoundPlayer::~SoundPlayer() {
}
