#include "RealTimeAudioServer.hpp"
#include "StereoInputProcess.hpp"
#include "StereoOutputProcess.hpp"

#include <cmath>

using namespace mpc::engine::audio::server;

RealTimeAudioServer::RealTimeAudioServer()
{
    previousMpcMonoInputChannelIndices.reserve(2);
}

void RealTimeAudioServer::start()
{
    if (running.load())
    {
        return;
    }

    running.store(true);
}

void RealTimeAudioServer::stop()
{
    running.store(false);
}

bool RealTimeAudioServer::isRunning()
{
    return running.load();
}

void RealTimeAudioServer::close()
{
    activeInputs.clear();
    activeOutputs.clear();
}

void RealTimeAudioServer::resizeBuffers(const int newSize)
{
    for (const auto &i : activeInputs)
    {
        if (i->localBuffer.size() != newSize * 2)
        {
            i->localBuffer.resize(newSize * 2);
        }
    }

    for (const auto &o : activeOutputs)
    {
        if (o->localBuffer.size() != newSize * 2)
        {
            o->localBuffer.resize(newSize * 2);
        }
    }

    AudioServer::resizeBuffers(newSize);
}

void RealTimeAudioServer::work(
    const float *const *inputBuffer, float *const *outputBuffer,
    const int nFrames, const std::vector<int8_t> &mpcMonoInputChannelIndices,
    const std::vector<int8_t> &mpcMonoOutputChannelIndices,
    const std::vector<int8_t> &hostInputChannelIndices,
    const std::vector<int8_t> &hostOutputChannelIndices)
{
    if (!running.load())
    {
        return;
    }

    if (previousMpcMonoInputChannelIndices != mpcMonoInputChannelIndices)
    {
        for (const auto &input : activeInputs)
        {
            for (int frame = 0; frame < nFrames * 2; ++frame)
            {
                input->localBuffer[frame] = 0.f;
            }
        }

        previousMpcMonoInputChannelIndices = mpcMonoInputChannelIndices;
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
                frame * 2 + localBufferFrameOffset;
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
                frame * 2 + localBufferFrameOffset;
            hostBuffer[frame] = vmpcOutput->localBuffer[localBufferFrameIndex];
        }
    }
}

void RealTimeAudioServer::setClient(
    const std::shared_ptr<AudioClient> clientToUse)
{
    client = clientToUse;
}

std::shared_ptr<IOAudioProcess>
RealTimeAudioServer::openAudioOutput(const std::string name)
{
    activeOutputs.emplace_back(std::make_shared<StereoOutputProcess>(name));
    return activeOutputs.back();
}

std::shared_ptr<IOAudioProcess>
RealTimeAudioServer::openAudioInput(const std::string name)
{
    activeInputs.emplace_back(std::make_shared<StereoInputProcess>(name));
    return activeInputs.back();
}

void RealTimeAudioServer::closeAudioOutput(
    const std::shared_ptr<IOAudioProcess> output)
{
    if (!output)
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

void RealTimeAudioServer::closeAudioInput(
    const std::shared_ptr<IOAudioProcess> input)
{
    if (!input)
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
