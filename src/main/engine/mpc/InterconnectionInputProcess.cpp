#include "InterconnectionInputProcess.hpp"

#include "MpcMixerInterconnection.hpp"
#include <engine/audio/core/AudioBuffer.hpp>

using namespace ctoot::mpc;

InterconnectionInputProcess::InterconnectionInputProcess(MpcMixerInterconnection* mmi, ctoot::audio::core::AudioBuffer* sharedBuffer)
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
