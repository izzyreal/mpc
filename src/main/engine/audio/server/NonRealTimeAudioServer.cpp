#include "engine/audio/server/NonRealTimeAudioServer.hpp"

#include "engine/EngineHost.hpp"
#include "engine/audio/core/AudioBuffer.hpp"
#include "engine/audio/server/AudioClient.hpp"
#include "engine/audio/server/AudioServer.hpp"
#include "engine/audio/server/IOAudioProcess.hpp"
#include "engine/audio/server/RealTimeAudioServer.hpp"
#include <utility>

using namespace mpc::engine::audio::server;
using namespace mpc::engine::audio::core;

NonRealTimeAudioServer::NonRealTimeAudioServer(
    EngineHost *engineHost, std::shared_ptr<AudioServer> serverToUse)
    : engineHost(engineHost), server(std::move(serverToUse))
{
}

void NonRealTimeAudioServer::setSampleRate(const int rate)
{
    server->setSampleRate(rate);
}

void NonRealTimeAudioServer::setRealTime(const bool rt)
{
    if (!isRunning())
    {
        realTime.store(rt);
        return;
    }

    if (realTime.load() != rt)
    {
        stop();

        realTime.store(rt);

        for (auto &buffer : server->getBuffers())
        {
            buffer->setRealTime(realTime);
        }

        start();
    }
}

bool NonRealTimeAudioServer::isRealTime() const
{
    return realTime.load();
}

void NonRealTimeAudioServer::start()
{
    if (isRunning())
    {
        return;
    }

    if (realTime.load())
    {
        server->start();
    }
    else
    {
        startNonRealTimeThread();
    }
}

void NonRealTimeAudioServer::startNonRealTimeThread()
{
    if (nonRealTimeThread.joinable())
    {
        nonRealTimeThread.join();
    }

    nonRealTimeThread = std::thread(
        [this]
        {
            runNonRealTime();
        });
}

void NonRealTimeAudioServer::stop()
{
    if (!isRunning())
    {
        return;
    }

    if (realTime.load())
    {
        server->stop();
    }
    else if (isRunningNonRealTime)
    {
        stopNonRealTimeThread();
    }
}

void NonRealTimeAudioServer::stopNonRealTimeThread()
{
    isRunningNonRealTime.store(false);

    if (nonRealTimeThread.joinable())
    {
        nonRealTimeThread.join();
    }
}

void NonRealTimeAudioServer::close()
{
    server->close();
    server = nullptr;
}

void NonRealTimeAudioServer::setClient(
    const std::shared_ptr<AudioClient> clientToUse)
{
    server->setClient(shared_from_this());
    client = clientToUse;
}

bool NonRealTimeAudioServer::isRunning()
{
    return realTime.load() ? server->isRunning() : isRunningNonRealTime.load();
}

void NonRealTimeAudioServer::work(
    const float *const *inputBuffer, float *const *outputBuffer,
    const int nFrames, const std::vector<int8_t> &mpcMonoInputChannelIndices,
    const std::vector<int8_t> &mpcMonoOutputChannelIndices,
    const std::vector<int8_t> &hostInputChannelIndices,
    const std::vector<int8_t> &hostOutputChannelIndices) const
{
    const auto realTimeAudioServer =
        std::dynamic_pointer_cast<RealTimeAudioServer>(server);

    realTimeAudioServer->work(
        inputBuffer, outputBuffer, nFrames, mpcMonoInputChannelIndices,
        mpcMonoOutputChannelIndices, hostInputChannelIndices,
        hostOutputChannelIndices);
}

void NonRealTimeAudioServer::work(const int nFrames)
{
    client->work(nFrames);
}

void NonRealTimeAudioServer::runNonRealTime()
{
    isRunningNonRealTime.store(true);

    while (isRunningNonRealTime.load())
    {
        const auto bufferSize = server->getBufferSize();
        engineHost->prepareProcessBlock(bufferSize);
        work(bufferSize);
    }
}

void NonRealTimeAudioServer::removeAudioBuffer(
    const std::shared_ptr<AudioBuffer> buffer)
{
    server->removeAudioBuffer(buffer);
}

std::shared_ptr<AudioBuffer>
NonRealTimeAudioServer::createAudioBuffer(const std::string name)
{
    return server->createAudioBuffer(name);
}

std::shared_ptr<IOAudioProcess>
NonRealTimeAudioServer::openAudioOutput(const std::string name)
{
    return server->openAudioOutput(name);
}

std::shared_ptr<IOAudioProcess>
NonRealTimeAudioServer::openAudioInput(const std::string name)
{
    return server->openAudioInput(name);
}

void NonRealTimeAudioServer::closeAudioOutput(
    const std::shared_ptr<IOAudioProcess> output)
{
    server->closeAudioOutput(output);
}

void NonRealTimeAudioServer::closeAudioInput(
    const std::shared_ptr<IOAudioProcess> input)
{
    server->closeAudioInput(input);
}

float NonRealTimeAudioServer::getSampleRate()
{
    return server->getSampleRate();
}

void NonRealTimeAudioServer::resizeBuffers(const int newSize)
{
    server->resizeBuffers(newSize);
}

NonRealTimeAudioServer::~NonRealTimeAudioServer()
{
    if (nonRealTimeThread.joinable())
    {
        nonRealTimeThread.join();
    }
}
