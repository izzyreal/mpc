#include <ctootextensions/InterconnectionOutputProcess.hpp>
#include <ctootextensions/MpcMixerInterconnection.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <audio/mixer/MixerControlsFactory.hpp>

using namespace mpc::ctootextensions;

InterconnectionOutputProcess::InterconnectionOutputProcess(MpcMixerInterconnection* mmi, ctoot::audio::core::AudioBuffer* sharedBuffer)
    : mmi(mmi)
    , sharedBuffer(sharedBuffer)
{
}

int InterconnectionOutputProcess::processAudio(ctoot::audio::core::AudioBuffer* buffer)
{
    buffer->copyFrom(sharedBuffer);
    return AUDIO_OK;
}

InterconnectionOutputProcess::~InterconnectionOutputProcess() {
}
