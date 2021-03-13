#pragma once
#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioProcess.hpp>

#include <sampler/Sound.hpp>

#include <collections/TSCircularBuffer.hpp>
#include <thirdp/libsamplerate/samplerate.h>

#include <fstream>
#include <memory>
#include <atomic>
#include <mutex>

using namespace mpc::sampler;
using namespace ctoot::audio::core;

namespace mpc::audiomidi
{
	class SoundPlayer
		: public AudioProcess
	{

	private:
		int ingestedSourceFrameCount = 0;
		int sourceFrameCount = 0;
		std::shared_ptr<AudioFormat> sourceFormat;
		bool isWav = false;
		bool isSnd = false;
		float fadeFactor = -1.0f;
		bool stopEarly = false;

	private:
		std::mutex _playing;
		bool playing = false;
		std::string filePath = "";
		circular_buffer<float> resampleInputBufferLeft = circular_buffer<float>(20000);
		circular_buffer<float> resampleInputBufferRight = circular_buffer<float>(20000);
		circular_buffer<float> resampleOutputBufferLeft = circular_buffer<float>(20000);
		circular_buffer<float> resampleOutputBufferRight = circular_buffer<float>(20000);
		SRC_STATE* srcLeft = nullptr;
		SRC_STATE* srcRight = nullptr;
		int srcLeftError = 0;
		int srcRightError = 0;
		std::ifstream stream;

	public:
		bool start(const std::string& filePath);
		int processAudio(ctoot::audio::core::AudioBuffer* buf, int nFrames) override;
		void open() {};
		void close() {};
		
	public:
		void enableStopEarly();

	private:
		void stop();
		void resampleChannel(bool left, std::vector<float>* input, int sourceSampleRate, int destinationSampleRate, bool endOfInput);

	public:
		SoundPlayer();
		~SoundPlayer();

	};
}
