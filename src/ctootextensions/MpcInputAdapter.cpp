#include <ctootextensions/MpcInputAdapter.hpp>
#include <audio/core/MetaInfo.hpp>
#include <audio/core/AudioBuffer.hpp>
#include <audio/core/AudioProcess.hpp>

using namespace mpc::ctootextensions;

MpcInputAdapter::MpcInputAdapter(ctoot::audio::core::AudioProcess* process)
	: ctoot::audio::core::AudioProcessAdapter(process)
{
	this->audioProcess = process;
}

int MpcInputAdapter::processAudio(ctoot::audio::core::AudioBuffer* buf)
{
    return super::processAudio(buf);
}

ctoot::audio::core::AudioProcess* MpcInputAdapter::getProcess()
{
    return audioProcess;
}
