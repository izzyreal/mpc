#pragma once

#include <engine/audio/server/AudioServer.hpp>
#include <engine/audio/server/AudioClient.hpp>

#include <thread>

#include <string>
#include <vector>

namespace mpc::engine::audio::server {

    class NonRealTimeAudioServer
            : public AudioServer, public AudioClient
    {

    private:

        std::shared_ptr<NonRealTimeAudioServer> me;

        bool realTime{true};
        bool isRunning_{false};
        std::shared_ptr<AudioServer> server;
        std::shared_ptr<AudioClient> client;
        std::thread nrtThread;
        bool startASAP{false};

    private:
        static void static_nrts(void *args);

    private:
        void run();

        void startNRT();

        void stopNRT();

    public:
        void setRealTime(bool rt);

        bool isRealTime();

        void setWeakPtr(std::shared_ptr<NonRealTimeAudioServer> sharedPtr);

    public:
        // implement AudioServer
        void start() override;

        void stop() override;

        void close() override;

        void setClient(std::shared_ptr<AudioClient> client) override;

        bool isRunning() override;

        mpc::engine::audio::core::AudioBuffer *createAudioBuffer(std::string name) override;

        void removeAudioBuffer(mpc::engine::audio::core::AudioBuffer *buffer) override;

        std::vector<std::string> getAvailableOutputNames() override;

        std::vector<std::string> getAvailableInputNames() override;

        IOAudioProcess *openAudioOutput(std::string name) override;

        IOAudioProcess *openAudioInput(std::string name) override;

        void closeAudioOutput(IOAudioProcess *output) override;

        void closeAudioInput(IOAudioProcess *input) override;

        float getSampleRate() override;

        void setSampleRate(int rate) override;

        int getInputLatencyFrames() override;

        int getOutputLatencyFrames() override;

        int getTotalLatencyFrames() override;

        void resizeBuffers(int newSize) override;

    public:
        void work(int nFrames) override;

    public:
        // For compatibility with JUCE 7.0.2
        void work(const float **inputBuffer, float **outputBuffer, int nFrames, int inputChannelCount,
                  int outputChannelCount);

        // For compatibility with JUCE 7.0.5
        void work(const float *const *inputBuffer, float *const *outputBuffer, int nFrames, int inputChannelCount,
                  int outputChannelCount);

        //For compatibility with the PortAudio framework
        void work(float *inputBuffer, float *outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount);

    public:
        NonRealTimeAudioServer(std::shared_ptr<AudioServer> server);

        ~NonRealTimeAudioServer();
    };

}
