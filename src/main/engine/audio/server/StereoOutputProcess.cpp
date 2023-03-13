#include <engine/audio/server/StereoOutputProcess.hpp>

#include <engine/audio/core/AudioBuffer.hpp>

using namespace ctoot::audio::server;
using namespace std;

StereoOutputProcess::StereoOutputProcess(string name)
	: AudioServerProcess(name)
{
}

int StereoOutputProcess::processAudio(ctoot::audio::core::AudioBuffer* buffer, int nFrames) {
	
	if (!buffer->isRealTime()) {
		return AudioProcess::AUDIO_OK;
	}

	if (buffer->getChannelFormat() != format) {
		buffer->setChannelFormat(format);
	}
	
	auto& left = buffer->getChannel(0);
	auto& right = buffer->getChannel(1);
		
	int frameCounter = 0;
	
	for (int i = 0; i < nFrames * 2; i += 2) {
		localBuffer[i] = left[frameCounter];
		localBuffer[i+1] = right[frameCounter++];
	}

	return AudioProcess::AUDIO_OK;
}
