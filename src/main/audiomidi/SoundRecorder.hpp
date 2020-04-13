#pragma once
#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioProcess.hpp>

#include <sampler/Sound.hpp>

#include <io/TSCircularBuffer.hpp>
#include <thirdp/libsamplerate/samplerate.h>

#include <memory>

using namespace mpc::sampler;
using namespace ctoot::audio::core;
using namespace std;


/*
* A SoundRecorder always records at 44.1KHz, like the real MPC2000XL.
*/

namespace mpc::audiomidi {

	class SoundRecorder
		: public AudioProcess
	{

	private:
		bool recording = false;
		int recordedFrameCount = 0;
		int lengthInFrames = 0;
		int mode = 0;
		weak_ptr<Sound> sound;
		circular_buffer<float> resampleBufferLeft = circular_buffer<float>(10000);
		circular_buffer<float> resampleBufferRight = circular_buffer<float>(10000);
		SRC_STATE* srcLeft = NULL;
		SRC_STATE* srcRight = NULL;
		int srcLeftError = 0;
		int srcRightError = 0;

	public:
		void prepare(const weak_ptr<Sound>, int lengthInFrames, int mode);
		void start();
		void stop();
		int processAudio(ctoot::audio::core::AudioBuffer* buf) override;
		void open() {};
		void close() {};

	private:
		void initSrc();
		vector<float> resampleChannel(bool left, vector<float>* input, int sourceSampleRate);

	public:
		SoundRecorder();
		~SoundRecorder();

	};
}
