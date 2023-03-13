#include "InterconnectionOutputProcess.hpp"

#include <engine/audio/core/AudioBuffer.hpp>

#include <engine/audio/mixer/MixerControlsFactory.hpp>

using namespace ctoot::mpc;

InterconnectionOutputProcess::InterconnectionOutputProcess(ctoot::audio::core::AudioBuffer* sharedBuffer)
    : sharedBuffer(sharedBuffer)
{
}

int InterconnectionOutputProcess::processAudio(ctoot::audio::core::AudioBuffer* buffer)
{
    buffer->copyFrom(sharedBuffer);
    return AUDIO_OK;
}
