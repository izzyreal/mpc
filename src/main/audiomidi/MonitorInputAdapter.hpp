#pragma once

#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <string>

namespace mpc::audiomidi {

	class MonitorInputAdapter
		: public ctoot::audio::core::AudioProcessAdapter
	{

	public:
		int32_t processAudio(ctoot::audio::core::AudioBuffer* buffer) override;
		MonitorInputAdapter(ctoot::audio::core::AudioProcess* process);

	};
}
