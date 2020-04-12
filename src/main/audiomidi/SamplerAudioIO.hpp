#pragma once

#include "MonitorOutput.hpp"

#include <Mpc.hpp>

#include <audio/server/AudioClient.hpp>
#include <audio/system/AudioDevice.hpp>
#include <audio/server/IOAudioProcess.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <memory>
#include <vector>
#include <string>

using namespace ctoot::audio::server;
using namespace ctoot::audio::system;
using namespace std;

namespace mpc::audiomidi {
	
	class SamplerAudioIO :
		public virtual AudioClient
		, public virtual AudioDevice {

	public:
		string getName() override;
		vector<weak_ptr<AudioInput>> getAudioInputs() override;
		vector<weak_ptr<AudioOutput>> getAudioOutputs() override;
		void closeAudio() override;

	public:
		void work(int nFrames) override;
		void setEnabled(bool b) override;

	private:
		shared_ptr<MonitorOutput> monitorOutput{};
		bool enabled = false;

	private:
		void changeSampleRate(int newRate);

	public:
		SamplerAudioIO(mpc::Mpc*);
		~SamplerAudioIO();

	};

}
