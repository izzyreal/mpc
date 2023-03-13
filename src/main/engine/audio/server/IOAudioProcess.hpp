#pragma once
#include <engine/audio/core/AudioProcess.hpp>

#include <vector>

namespace ctoot::audio::server
{
	class IOAudioProcess
		: public ctoot::audio::core::AudioProcess
	{
	public:
		std::vector<float> localBuffer;
		virtual std::shared_ptr<ctoot::audio::core::ChannelFormat> getChannelFormat() = 0;
		virtual std::string getName() = 0;
        virtual ~IOAudioProcess() {}

	};
}
