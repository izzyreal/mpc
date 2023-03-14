#pragma once
#include <engine/audio/core/AudioProcess.hpp>

#include <vector>

namespace mpc::engine::audio::server
{
	class IOAudioProcess
		: public mpc::engine::audio::core::AudioProcess
	{
	public:
		std::vector<float> localBuffer;
		virtual std::shared_ptr<mpc::engine::audio::core::ChannelFormat> getChannelFormat() = 0;
		virtual std::string getName() = 0;
        virtual ~IOAudioProcess() {}

	};
}
