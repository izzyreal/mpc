#include "InterconnectionOutputProcess.hpp"

#include "engine/audio/core/AudioBuffer.hpp"

#include "engine/audio/mixer/MixerControlsFactory.hpp"

using namespace mpc::engine;

InterconnectionOutputProcess::InterconnectionOutputProcess(mpc::engine::audio::core::AudioBuffer* sharedBuffer)
    : sharedBuffer(sharedBuffer)
{
}

int InterconnectionOutputProcess::processAudio(mpc::engine::audio::core::AudioBuffer* buffer)
{
    buffer->copyFrom(sharedBuffer);
    return AUDIO_OK;
}
