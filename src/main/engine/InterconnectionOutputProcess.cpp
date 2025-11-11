#include "InterconnectionOutputProcess.hpp"

#include "engine/audio/core/AudioBuffer.hpp"

using namespace mpc::engine;

InterconnectionOutputProcess::InterconnectionOutputProcess(
    audio::core::AudioBuffer *sharedBuffer)
    : sharedBuffer(sharedBuffer)
{
}

int InterconnectionOutputProcess::processAudio(audio::core::AudioBuffer *buffer)
{
    buffer->copyFrom(sharedBuffer);
    return AUDIO_OK;
}
