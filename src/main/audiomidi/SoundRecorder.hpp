#pragma once
#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioProcess.hpp>

#include <sampler/Sound.hpp>

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

	public:
		void prepare(const weak_ptr<Sound>, int lengthInFrames, int mode);
		void start();
		void stop();
		int processAudio(ctoot::audio::core::AudioBuffer* buf) override;
		void open() {};
		void close() {};

	public:
		SoundRecorder();
		~SoundRecorder();

	};
}
