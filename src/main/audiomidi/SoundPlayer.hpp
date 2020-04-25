#pragma once
#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioProcess.hpp>

#include <sampler/Sound.hpp>

#include <io/TSCircularBuffer.hpp>
#include <thirdp/libsamplerate/samplerate.h>

#include <fstream>
#include <memory>
#include <atomic>

using namespace mpc::sampler;
using namespace ctoot::audio::core;
using namespace std;

namespace mpc::audiomidi {

	class SoundPlayer
		: public AudioProcess
	{

	private:
		int sourceFrameCount = 0;
	
	private:
		atomic<bool> playing = ATOMIC_VAR_INIT(false);
		int playedSourceFrameCount = 0;
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
		shared_ptr<AudioFormat> sourceFormat;
		bool isWav = false;
		bool isSnd = false;

	public:
		void start(const string& filePath);
		void stop();
		int processAudio(ctoot::audio::core::AudioBuffer* buf) override;
		void open() {};
		void close() {};
		bool isPlaying();

	private:
		void initSrc();
		void resampleChannel(bool left, vector<float>* input, int sourceSampleRate, int destinationSampleRate);

	public:
		SoundPlayer();
		~SoundPlayer();

	};
}
