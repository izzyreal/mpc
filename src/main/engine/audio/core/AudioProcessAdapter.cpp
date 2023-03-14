#include <engine/audio/core/AudioProcessAdapter.hpp>
#include <engine/audio/core/AudioProcess.hpp>

using namespace mpc::engine::audio::core;

AudioProcessAdapter::AudioProcessAdapter(AudioProcess* process)
{
	this->process = process;
}

int AudioProcessAdapter::processAudio(AudioBuffer* buf, int nFrames)
{
	return process->processAudio(buf, nFrames);
}
