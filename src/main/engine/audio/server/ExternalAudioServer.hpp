/**
* An AudioServer implementation that can be used with any externally provided audio API.
**/

#pragma once
#include <engine/audio/server/AudioServer.hpp>

#include <string>
#include <vector>
#include <memory>

namespace mpc::engine::audio::server
{
	class ExternalAudioServer : public AudioServer {

	public:
		void start() override;
		void stop() override;
		bool isRunning() override;
		void close() override;
		void setClient(std::shared_ptr<AudioClient> client) override;
		std::vector<std::string> getAvailableOutputNames() override;
		std::vector<std::string> getAvailableInputNames() override;
		IOAudioProcess* openAudioOutput(std::string name) override;
		IOAudioProcess* openAudioInput(std::string name) override;
		void closeAudioOutput(mpc::engine::audio::server::IOAudioProcess* output) override;
		void closeAudioInput(mpc::engine::audio::server::IOAudioProcess* input) override;
		int getInputLatencyFrames() override;
		int getOutputLatencyFrames() override;
		int getTotalLatencyFrames() override;
		void resizeBuffers(int newSize) override;

	public:
		//For compatibility with the PortAudio framework
		void work(float* inputBuffer, float* outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount);
		
		// For compatibility with JUCE 7.0.2
        void work(const float** inputBuffer, float** outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount);

		// For compatibility with JUCE 7.0.5
        void work(const float* const* inputBuffer, float* const* outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount);
	};
}
