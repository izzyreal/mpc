#pragma once
#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioProcess.hpp>

#include <sampler/Sound.hpp>

#include <io/TSCircularBuffer.hpp>
#include <thirdp/libsamplerate/samplerate.h>

#include <fstream>
#include <memory>
#include <atomic>
#include <mutex>

using namespace mpc::sampler;
using namespace ctoot::audio::core;
using namespace std;

namespace mpc::audiomidi {

	class SoundPlayer
		: public AudioProcess
	{

	private:
		int ingestedSourceFrameCount = 0;
		int sourceFrameCount = 0;
		shared_ptr<AudioFormat> sourceFormat;
		bool isWav = false;
		bool isSnd = false;

	private:
		mutex _playing;
		bool playing = false;
		string filePath = "";
		circular_buffer<float> resampleInputBufferLeft = circular_buffer<float>(20000);
		circular_buffer<float> resampleInputBufferRight = circular_buffer<float>(20000);
		circular_buffer<float> resampleOutputBufferLeft = circular_buffer<float>(20000);
		circular_buffer<float> resampleOutputBufferRight = circular_buffer<float>(20000);
		SRC_STATE* srcLeft = nullptr;
		SRC_STATE* srcRight = nullptr;
		int srcLeftError = 0;
		int srcRightError = 0;
		ifstream stream;

	public:
		void start(const string& filePath);
		void stop();
		int processAudio(ctoot::audio::core::AudioBuffer* buf) override;
		void open() {};
		void close() {};

	private:
		void resampleChannel(bool left, vector<float>* input, int sourceSampleRate, int destinationSampleRate, bool endOfInput);

	public:
		SoundPlayer();
		~SoundPlayer();

	};
}
