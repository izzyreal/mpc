#include "engine/audio/server/NonRealTimeAudioServer.hpp"

#include "engine/audio/core/AudioBuffer.hpp"
#include "engine/audio/server/AudioClient.hpp"
#include "engine/audio/server/AudioServer.hpp"
#include "engine/audio/server/IOAudioProcess.hpp"
#include "engine/audio/server/RealTimeAudioServer.hpp"
#include <utility>

using namespace mpc::engine::audio::server;
using namespace mpc::engine::audio::core;
using namespace std;

NonRealTimeAudioServer::NonRealTimeAudioServer(
    shared_ptr<AudioServer> serverToUse)
{
    server = std::move(serverToUse);
}

void NonRealTimeAudioServer::setSampleRate(int rate)
{
    server->setSampleRate(rate);
}

void NonRealTimeAudioServer::setSharedPtr(
    shared_ptr<NonRealTimeAudioServer> sharedPtr)
{
    me = std::move(sharedPtr);
}

void NonRealTimeAudioServer::setRealTime(bool rt)
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

void NonRealTimeAudioServer::setClient(shared_ptr<AudioClient> clientToUse)
{
    server->setClient(me);
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
    auto realTimeAudioServer =
        dynamic_pointer_cast<RealTimeAudioServer>(server);
    realTimeAudioServer->work(
        inputBuffer, outputBuffer, nFrames, mpcMonoInputChannelIndices,
        mpcMonoOutputChannelIndices, hostInputChannelIndices,
        hostOutputChannelIndices);
}

void NonRealTimeAudioServer::work(int nFrames)
{
    client->work(nFrames);
}

void NonRealTimeAudioServer::runNonRealTime()
{
    isRunningNonRealTime.store(true);

    while (isRunningNonRealTime.load())
    {
        work(server->getBufferSize());
    }
}

void NonRealTimeAudioServer::removeAudioBuffer(AudioBuffer *buffer)
{
    server->removeAudioBuffer(buffer);
}

AudioBuffer *NonRealTimeAudioServer::createAudioBuffer(string name)
{
    return server->createAudioBuffer(name);
}

IOAudioProcess *NonRealTimeAudioServer::openAudioOutput(string name)
{
    return server->openAudioOutput(name);
}

IOAudioProcess *NonRealTimeAudioServer::openAudioInput(string name)
{
    return server->openAudioInput(name);
}

void NonRealTimeAudioServer::closeAudioOutput(IOAudioProcess *output)
{
    server->closeAudioOutput(output);
}

void NonRealTimeAudioServer::closeAudioInput(IOAudioProcess *input)
{
    server->closeAudioInput(input);
}

float NonRealTimeAudioServer::getSampleRate()
{
    return server->getSampleRate();
}

void NonRealTimeAudioServer::resizeBuffers(int newSize)
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
