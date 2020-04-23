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

	if (wav_readHeader(stream, sourceSampleRate, validBits, sourceNumChannels, dataChunkSize, numFrames)) {
		sourceFormat = make_shared<AudioFormat>(sourceSampleRate, validBits, sourceNumChannels, true, false);
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

	playing.store(false);

	if (srcLeft != NULL) {
		src_delete(srcLeft);
		srcLeft = NULL;
	}

	if (srcRight != NULL) {
		src_delete(srcRight);
		srcRight = NULL;
	}
}

int SoundPlayer::processAudio(AudioBuffer* buf)
{
	auto left = buf->getChannel(0);
	auto right = buf->getChannel(1);

	MLOG("");
	buf->makeSilence();

	try {
		if (playing.load()) {

			//auto resample = buf->getSampleRate() != sourceSampleRate;
			bool resample = true;

			if (resample) {
				if (srcLeft == NULL || srcRight == NULL) {
					initSrc();
				}
			}

			vector<char> byteBuffer(buf->getByteArrayBufferSize(sourceFormat.get()));
			wav_read_bytes(stream, byteBuffer);

			auto sourceBuffer = make_shared<AudioBuffer>("temp", 2, buf->getSampleCount(), sourceFormat->getSampleRate());
			sourceBuffer->initFromByteArray_(byteBuffer, 0, byteBuffer.size(), sourceFormat.get());

			/*
			if (sourceBuffer->isSilent()) {
				MLOG("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!sourceBuffer is silent");
			}
			else {
				MLOG("sourceBuffer is ok :):):):):):):):):):):):):):)");
			}
			*/

			if (sourceFormat->getChannels() >= 1 && resample) {

				resampleChannel(true, sourceBuffer->getChannel(0), sourceFormat->getSampleRate(), buf->getSampleRate());

				//MLOG("available " + to_string(resampleOutputBufferLeft.available()));

				if (resampleOutputBufferLeft.available() < buf->getSampleCount()) {
					MLOG("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Not enough available");
					buf->makeSilence();
					return AUDIO_OK;
				}

				for (int i = 0; i < left->size(); i++) {
					auto val = resampleOutputBufferLeft.get();
					(*left)[i] = val;
					(*right)[i] = (*left)[i];
				}
				if (buf->isSilent()) {
					MLOG("Buffer is silent!");
				}
			}

			vector<float> resampledRight;

			if (sourceFormat->getChannels() >= 2 && resample) {
				//resampleChannel(false, sourceBuffer->getChannel(1), sourceSampleRate, buf->getSampleRate());
				for (int i = 0; i < left->size(); i++) {
					//(*right)[i] = resampleOutputBufferRight.get();
				}
			}

			if (playedFrameCount >= numFrames) {
				MLOG("Stopping...");
				playing.store(false);
			}

			return AUDIO_OK;
		}
	}
	catch (const exception& e) {
		auto msg = e.what();
		MLOG(msg);
	}
	
	MLOG("Making the buffer silent...!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	buf->makeSilence();
	return AUDIO_SILENCE;
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
	if (sourceFormat->getChannels() >= 1) {
		srcLeft = src_new(0, 1, &srcLeftError);
	}

	if (sourceFormat->getChannels() >= 2) {
		srcRight = src_new(0, 1, &srcRightError);
	}
}

SoundPlayer::~SoundPlayer() {
}
