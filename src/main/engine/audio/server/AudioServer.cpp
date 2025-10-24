#include "engine/audio/server/AudioServer.hpp"

using namespace mpc::engine::audio::server;
using namespace std;

AudioServer::AudioServer() {
}

float AudioServer::getSampleRate() {
	return sampleRate;
}

void mpc::engine::audio::server::AudioServer::setSampleRate(int rate) {
	sampleRate = static_cast<float>(rate);
	for (auto& b : buffers)
		b->setSampleRate(sampleRate);
}

const std::vector<mpc::engine::audio::core::AudioBuffer*>& AudioServer::getBuffers() {
	return buffers;
}

const unsigned int AudioServer::getBufferSize() {
	return bufferSize;
}

mpc::engine::audio::core::AudioBuffer* AudioServer::createAudioBuffer(string name) {
	buffers.push_back(new mpc::engine::audio::core::AudioBuffer(name, 2, bufferSize, sampleRate));
	return buffers.back();
}

void AudioServer::resizeBuffers(int newSize) {
	bufferSize = newSize;
	for (auto& b : buffers) {
		b->changeSampleCount(bufferSize, false);
	}
}

void AudioServer::removeAudioBuffer(mpc::engine::audio::core::AudioBuffer* buffer)
{
	for (int i = 0; i < buffers.size(); i++) {
		if (buffers[i] == buffer) {
            delete buffers[i];
			buffers.erase(buffers.begin() + i);
			break;
		}
	}
}

AudioServer::~AudioServer() {
	for (auto& b : buffers) {
        delete b;
	}
}
