/**
* The ExternalAudioServer expects someone else to talk to the host OS audio API's.
* 
* After instantiating an ExternalAudioServer and starting it, we can make it work(),
* optionally providing it with input and output buffers. After calling work, the input data
* has been processed and consumed by ctoot, and ctoot will have filled the output data with
* AudioMixer's main output data.
**/

#pragma once
#include <engine/audio/server/AudioServer.hpp>

#include <string>
#include <vector>
#include <memory>

namespace ctoot::audio::server
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
		void closeAudioOutput(ctoot::audio::server::IOAudioProcess* output) override;
		void closeAudioInput(ctoot::audio::server::IOAudioProcess* input) override;
		int getInputLatencyFrames() override;
		int getOutputLatencyFrames() override;
		int getTotalLatencyFrames() override;
		void resizeBuffers(int newSize) override;

	public:
		void work() override;
		
		//For compatibility with the PortAudio framework
		void work(float* inputBuffer, float* outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount);
		
		// For compatibility with JUCE 7.0.2
        void work(const float** inputBuffer, float** outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount);

		// For compatibility with JUCE 7.0.5
        void work(const float* const* inputBuffer, float* const* outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount);
	};
}
