#include <engine/audio/server/AudioServerProcess.hpp>

#include <engine/audio/core/AudioBuffer.hpp>

namespace mpc::engine::audio::server
{
	class StereoOutputProcess
		: public AudioServerProcess
	{

	public:
		int processAudio(mpc::engine::audio::core::AudioBuffer* buffer, int nFrames) override;
		StereoOutputProcess(std::string name);

	};
}
