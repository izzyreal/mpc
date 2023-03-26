#include <engine/audio/server/AudioServer.hpp>
#include <utility>

using namespace mpc::engine::audio::server;
using namespace mpc::engine::audio::core;
using namespace std;

float AudioServer::getSampleRate() {
	return sampleRate;
}

void AudioServer::setSampleRate(int rate) {
	sampleRate = static_cast<float>(rate);
	for (auto& b : buffers)
		b->setSampleRate(sampleRate);
}

const std::vector<AudioBuffer*>& AudioServer::getBuffers() {
	return buffers;
}

unsigned int AudioServer::getBufferSize() const {
	return bufferSize;
}

AudioBuffer* AudioServer::createAudioBuffer(string name) {
	buffers.push_back(new AudioBuffer(std::move(name), 2, bufferSize, sampleRate));
	return buffers.back();
}

void AudioServer::resizeBuffers(int newSize) {
	bufferSize = newSize;
	for (auto& b : buffers) {
		b->changeSampleCount(bufferSize);
	}
}

void AudioServer::removeAudioBuffer(AudioBuffer* buffer)
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
		if (b != nullptr) {
			delete b;
		}
	}
}
