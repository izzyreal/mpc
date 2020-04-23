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

	if (wav_readHeader(stream, sourceSampleRate, validBits, numChannels, dataChunkSize, numFrames)) {
		format = make_shared<AudioFormat>(sourceSampleRate, validBits, numChannels, true, false);
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

	try {
		if (playing.load()) {

			//auto resample = buf->getSampleRate() != sourceSampleRate;
			bool resample = true;

			if (resample) {
				if (srcLeft == NULL || srcRight == NULL) {
					initSrc();
				}
			}

			vector<char> byteBuffer(buf->getByteArrayBufferSize(format.get()));
			wav_read_bytes(stream, byteBuffer);
			auto sourceBuffer = make_shared<AudioBuffer>("temp", 2, 0, sourceSampleRate);
			sourceBuffer->initFromByteArray_(byteBuffer, 0, byteBuffer.size(), format.get());

			if (numChannels >= 1 && resample) {
				resampleChannel(true, sourceBuffer->getChannel(0), sourceSampleRate, buf->getSampleRate());

				for (int i = 0; i < left->size(); i++) {
					if (resampleOutputBufferLeft.empty()) {
						// If this happens it will cause a glitch. We must implement an alternative getSize on circular_buffer that returns the difference between head and tail.
						// Then we can skip the first processAudio call if it's not full enough. This would cause latency. If it's too much we can implement the same linear interpolation as in MpcVoice.
						break;
					}
					else {
						(*left)[i] = resampleOutputBufferLeft.get();
					}
				}
			}

			vector<float> resampledRight;

			if (numChannels >= 2 && resample) {
				//resampleChannel(false, sourceBuffer->getChannel(1), sourceSampleRate, buf->getSampleRate());
				for (int i = 0; i < left->size(); i++) {
					if (resampleOutputBufferRight.empty()) {
						break;
					}
					else {
						(*right)[i] = resampleOutputBufferRight.get();
					}
				}
			}

			if (playedFrameCount >= numFrames) {
				playing.store(false);
			}

			return AUDIO_OK;
		}
	}
	catch (const exception& e) {
		auto msg = e.what();
		MLOG(msg);
	}

	buf->makeSilence();
	return AUDIO_SILENCE;
}

void SoundPlayer::resampleChannel(bool left, vector<float>* inputBuffer, int sourceSampleRate, int destinationSampleRate)
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
	
	output.resize(data.output_frames_gen);

	for (auto f : output) {
		circularOutputBuffer->put(f);
	}
}

void SoundPlayer::initSrc() {
	if (numChannels >= 1) {
		srcLeft = src_new(0, 1, &srcLeftError);
	}
	
	if (!numChannels >= 2) {
		srcRight = src_new(0, 1, &srcRightError);
	}
}

SoundPlayer::~SoundPlayer() {
}
