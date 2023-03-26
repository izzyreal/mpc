#pragma once
#include <engine/audio/core/AudioProcessAdapter.hpp>
#include <engine/audio/core/AudioProcess.hpp>

#include <sampler/Sound.hpp>

#include <collections/TSCircularBuffer.hpp>
#include <thirdp/libsamplerate/samplerate.h>

#include <observer/Observable.hpp>

#include <memory>
#include <atomic>

using namespace mpc::sampler;
using namespace mpc::engine::audio::core;

/*
* A SoundRecorder always records at 44.1KHz, like the real MPC2000XL.
*/

namespace mpc { class Mpc; }

namespace mpc::audiomidi
{
	class SoundRecorder
		: public AudioProcess
		, public moduru::observer::Observable
	{

	private:
		mpc::Mpc& mpc;
		bool recording = false;
		bool cancelled = false;
		int lengthInFrames = 0;
		int mode = 0;
		std::weak_ptr<Sound> sound;
		circular_buffer<float> resampleBufferLeft = circular_buffer<float>(10000);
		circular_buffer<float> resampleBufferRight = circular_buffer<float>(10000);
		
		// 100ms at 44.1KHz pre-rec buffer
		circular_buffer<float> preRecBufferLeft = circular_buffer<float>(10000);
		circular_buffer<float> preRecBufferRight = circular_buffer<float>(10000);
		
		SRC_STATE* srcLeft = nullptr;
		SRC_STATE* srcRight = nullptr;
		int srcLeftError = 0;
		int srcRightError = 0;
		unsigned int inputGain = 0;
		std::atomic<bool> sampleScreenActive = ATOMIC_VAR_INIT(false);
		bool lastSampleScreenActive = false;
		bool armed = false;
		void initSrc();
		std::valarray<float> resampleChannel(bool left, std::valarray<float>& input, int sourceSampleRate);

	public:
		void setSampleScreenActive(bool active);
		void prepare(const std::weak_ptr<Sound>, int newLengthInFrames);
		void start();
		void stop();
		void cancel();
		int processAudio(mpc::engine::audio::core::AudioBuffer* buf, int nFrames) override;
		bool isRecording();
		unsigned int getInputGain();
		void setInputGain(unsigned int);
		void setArmed(bool b);
		bool isArmed();

		SoundRecorder(mpc::Mpc& mpc);
		~SoundRecorder();

		void open() override {}
		void close() override {}
	};
}
