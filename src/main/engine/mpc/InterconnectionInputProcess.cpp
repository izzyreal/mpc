#include "InterconnectionInputProcess.hpp"

#include "MixerInterconnection.hpp"
#include <engine/audio/core/AudioBuffer.hpp>

using namespace ctoot::mpc;

InterconnectionInputProcess::InterconnectionInputProcess(MixerInterconnection* mmi, ctoot::audio::core::AudioBuffer* sharedBuffer)
    : mmi(mmi)
    , sharedBuffer(sharedBuffer)
{
}

int InterconnectionInputProcess::processAudio(ctoot::audio::core::AudioBuffer* buffer)
{
	sharedBuffer->copyFrom(buffer);
	if (!mmi->isLeftEnabled()) sharedBuffer->makeSilence(0);
	if (!mmi->isRightEnabled()) sharedBuffer->makeSilence(1);
	return AUDIO_OK;
}
