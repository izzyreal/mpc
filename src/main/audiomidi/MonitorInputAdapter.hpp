#pragma once

#include <engine/audio/core/AudioProcessAdapter.hpp>
#include <engine/audio/core/AudioBuffer.hpp>

#include <string>

namespace mpc {
	class Mpc;
}

namespace mpc::audiomidi {

	class MonitorInputAdapter
		: public mpc::engine::audio::core::AudioProcessAdapter
	{
	private:
		mpc::Mpc& mpc;

	public:
		int32_t processAudio(mpc::engine::audio::core::AudioBuffer* buffer, int nFrames) override;
		MonitorInputAdapter(mpc::Mpc& mpc, mpc::engine::audio::core::AudioProcess* process);

	};
}
