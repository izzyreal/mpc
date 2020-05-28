#pragma once

#include <audio/system/AudioOutput.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <string>

using namespace ctoot::audio::system;

namespace mpc::audiomidi {

	class MonitorOutput
		: public virtual AudioOutput
	{

	public:
		bool closed{ false };

	public:
		std::string getName() override;
		void open() override;
		int32_t processAudio(ctoot::audio::core::AudioBuffer* buffer) override;
		void close() override;
		std::string getLocation() override;

	};
}
