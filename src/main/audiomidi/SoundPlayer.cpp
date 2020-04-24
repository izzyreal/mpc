#include <audiomidi/SoundPlayer.hpp>

#include <audio/core/AudioFormat.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <cmath>

#include "WavInputFileStream.hpp"

using namespace std;
using namespace mpc::sampler;
using namespace mpc::audiomidi;

SoundPlayer::SoundPlayer()
{
}

// Should be called from the audio thread
void SoundPlayer::start(const string& filePath) {

	stream = wav_init_ifstream(filePath);

	int validBits;
	int dataChunkSize;
	int sourceSampleRate;
	int sourceNumChannels;

	if (wav_readHeader(stream, sourceSampleRate, validBits, sourceNumChannels, dataChunkSize, sourceFrameCount)) {
		
		sourceFormat = make_shared<AudioFormat>(sourceSampleRate, validBits, sourceNumChannels, true, false);
		
		resampleInputBufferLeft.reset();
		resampleInputBufferRight.reset();
		resampleOutputBufferLeft.reset();
		resampleOutputBufferRight.reset();

		playedSourceFrameCount = 0;

		playing.store(true);
	}
}

bool SoundPlayer::isPlaying() {
	return playing.load();
}

void SoundPlayer::stop() {

	if (!playing.load()) {
		return;
	}

	if (stream.is_open()) {
		wav_close(stream);
	}

	playing.store(false);

	if (srcLeft != nullptr) {
		src_delete(srcLeft);
		srcLeft = nullptr;
	}

	if (srcRight != nullptr) {
		src_delete(srcRight);
		srcRight = nullptr;
	}
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

	if (resample) {
		if (srcLeft == nullptr || (sourceFormat->getChannels() >= 2 && srcRight == nullptr)) {
			initSrc();
		}
	}

	bool shouldClose = false;
	auto samplesToRead = (int)ceil((sourceFormat->getSampleRate() / buf->getSampleRate()) * buf->getSampleCount());

	if (playedSourceFrameCount + samplesToRead >= sourceFrameCount) {
		shouldClose = true;
		samplesToRead = sourceFrameCount - playedSourceFrameCount;
	}

	auto sourceBuffer = make_shared<AudioBuffer>("temp", sourceFormat->getChannels(), samplesToRead, sourceFormat->getSampleRate());
	vector<char> byteBuffer(sourceBuffer->getByteArrayBufferSize(sourceFormat.get()));

	wav_read_bytes(stream, byteBuffer);
	sourceBuffer->initFromByteArray_(byteBuffer, 0, byteBuffer.size(), sourceFormat.get());

	if (shouldClose) {
		buf->makeSilence();
	}

	if (resample) {

		if (sourceFormat->getChannels() >= 1) {
			resampleChannel(true, sourceBuffer->getChannel(0), sourceFormat->getSampleRate(), buf->getSampleRate());
		}

		if (sourceFormat->getChannels() >= 2) {
			resampleChannel(false, sourceBuffer->getChannel(1), sourceFormat->getSampleRate(), buf->getSampleRate());
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
			sourceBuffer->addChannel(false);
			sourceBuffer->copyChannel(0, 1);
		}

		buf->copyFrom(sourceBuffer.get());
	}

	playedSourceFrameCount += samplesToRead;

	if (shouldClose) {
		stop();
	}

	return AUDIO_OK;
}

void SoundPlayer::resampleChannel(bool left, vector<float>* inputBuffer, int sourceSampleRate, int destinationSampleRate)
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
