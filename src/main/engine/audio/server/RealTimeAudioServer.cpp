#include "RealTimeAudioServer.hpp"
#include "StereoInputProcess.hpp"
#include "StereoOutputProcess.hpp"

#include <cmath>

using namespace mpc::engine::audio::server;
using namespace std;

void RealTimeAudioServer::start()
{
    if (running)
    {
        return;
    }

    running = true;
}

void RealTimeAudioServer::stop()
{
    running = false;
}

bool RealTimeAudioServer::isRunning()
{
    return running;
}

void RealTimeAudioServer::close()
{
    for (auto &i : activeInputs)
    {
        if (i != nullptr)
        {
            delete i;
        }
    }

    for (auto &o : activeOutputs)
    {
        if (o != nullptr)
        {
            delete o;
        }
    }

    activeInputs.clear();
    activeOutputs.clear();
}

void RealTimeAudioServer::resizeBuffers(int newSize)
{
    for (auto &i : activeInputs)
    {
        if (i->localBuffer.size() != newSize * 2)
        {
            i->localBuffer.resize(newSize * 2);
        }
    }

    for (auto &o : activeOutputs)
    {
        if (o->localBuffer.size() != newSize * 2)
        {
            o->localBuffer.resize(newSize * 2);
        }
    }

    AudioServer::resizeBuffers(newSize);
}

// For compatibility with the PortAudio framework
void RealTimeAudioServer::work(float *inputBuffer, float *outputBuffer,
                               int nFrames, int inputChannelCount,
                               int outputChannelCount) const
{
    if (!running)
    {
        return;
    }

    int sampleCounter = 0;
    const int inputsToProcess =
        min((int)(inputChannelCount * 0.5), (int)activeInputs.size());

    for (int frame = 0; frame < nFrames; frame++)
    {
        for (int input = 0; input < inputsToProcess; input++)
        {
            activeInputs[input]->localBuffer[sampleCounter++] = *inputBuffer++;
            activeInputs[input]->localBuffer[sampleCounter++] = *inputBuffer++;
        }
    }

    client->work(nFrames);

    const int outputsToProcess = outputChannelCount * 0.5;

    for (int frame = 0; frame < nFrames; frame++)
    {
        for (int output = 0; output < outputsToProcess; output++)
        {
            if (output >= activeOutputs.size())
            {
                *outputBuffer++ = 0.0f;
                *outputBuffer++ = 0.0f;
                continue;
            }

            *outputBuffer++ = activeOutputs[output]->localBuffer[(frame * 2)];
            *outputBuffer++ =
                activeOutputs[output]->localBuffer[(frame * 2) + 1];
        }
    }
}

// For compatibility with JUCE 7.0.5+
void RealTimeAudioServer::work(
    const float *const *inputBuffer, float *const *outputBuffer, int nFrames,
    const std::vector<int8_t> &mpcMonoInputChannelIndices,
    const std::vector<int8_t> &mpcMonoOutputChannelIndices,
    const std::vector<int8_t> &hostInputChannelIndices,
    const std::vector<int8_t> &hostOutputChannelIndices) const
{
    if (!running)
    {
        return;
    }

    for (int i = 0; i < mpcMonoInputChannelIndices.size(); i++)
    {
        const auto mpcStereoInputIndex =
            static_cast<int>(std::floor(mpcMonoInputChannelIndices[i] / 2.f));
        auto &vmpcInput = activeInputs[mpcStereoInputIndex];
        const auto localBufferFrameOffset = mpcMonoInputChannelIndices[i] % 2;
        const auto &hostBuffer = inputBuffer[hostInputChannelIndices[i]];

        for (int frame = 0; frame < nFrames; frame++)
        {
            const auto localBufferFrameIndex =
                (frame * 2) + localBufferFrameOffset;
            vmpcInput->localBuffer[localBufferFrameIndex] = hostBuffer[frame];
        }
    }

    client->work(nFrames);

    for (int i = 0; i < mpcMonoOutputChannelIndices.size(); i++)
    {
        const auto mpcStereoOutputIndex =
            static_cast<int>(std::floor(mpcMonoOutputChannelIndices[i] / 2.f));
        const auto &vmpcOutput = activeOutputs[mpcStereoOutputIndex];
        const auto localBufferFrameOffset = mpcMonoOutputChannelIndices[i] % 2;
        auto &hostBuffer = outputBuffer[hostOutputChannelIndices[i]];

        for (int frame = 0; frame < nFrames; frame++)
        {
            const auto localBufferFrameIndex =
                (frame * 2) + localBufferFrameOffset;
            hostBuffer[frame] = vmpcOutput->localBuffer[localBufferFrameIndex];
        }
    }
}

void RealTimeAudioServer::setClient(shared_ptr<AudioClient> client)
{
    this->client = client.get();
}

IOAudioProcess *RealTimeAudioServer::openAudioOutput(string name)
{
    activeOutputs.push_back(new StereoOutputProcess(name));
    return activeOutputs.back();
}

IOAudioProcess *RealTimeAudioServer::openAudioInput(string name)
{
    activeInputs.push_back(new StereoInputProcess(name));
    return activeInputs.back();
}

void RealTimeAudioServer::closeAudioOutput(IOAudioProcess *output)
{
    if (output == nullptr)
    {
        return;
    }

    output->close();

    for (int i = 0; i < activeOutputs.size(); i++)
    {
        if (activeOutputs[i] == output)
        {
            activeOutputs.erase(activeOutputs.begin() + i);
            break;
        }
    }
}

void RealTimeAudioServer::closeAudioInput(IOAudioProcess *input)
{
    if (input == nullptr)
    {
        return;
    }

    input->close();

    for (int i = 0; i < activeInputs.size(); i++)
    {
        if (activeInputs[i] == input)
        {
            activeInputs.erase(activeInputs.begin() + i);
            break;
        }
    }
}
