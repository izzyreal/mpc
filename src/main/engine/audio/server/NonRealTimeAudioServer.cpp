#include <engine/audio/server/NonRealTimeAudioServer.hpp>

#include <engine/audio/core/AudioBuffer.hpp>
#include <engine/audio/server/AudioClient.hpp>
#include <engine/audio/server/AudioServer.hpp>
#include <engine/audio/server/IOAudioProcess.hpp>
#include <engine/audio/server/ExternalAudioServer.hpp>

#include <stdio.h>

using namespace mpc::engine::audio::server;
using namespace std;

NonRealTimeAudioServer::NonRealTimeAudioServer(shared_ptr<AudioServer> server)
{
	this->server = server;
	realTime = true;
	isRunning_ = false;
	startASAP = false;
}

void NonRealTimeAudioServer::setSampleRate(int rate)
{
	server->setSampleRate(rate);
}

void NonRealTimeAudioServer::setSharedPtr(shared_ptr<NonRealTimeAudioServer> sharedPtr) {
	me = sharedPtr;
}

void NonRealTimeAudioServer::setRealTime(bool rt)
{	if (!isRunning()) {
		realTime = rt;
		return;
	}
	if (realTime != rt) {
		try {
			stop();
		}
		catch (const exception& e) {
            printf("%s", e.what());
		}
		realTime = rt;
		for (auto& buffer : server->getBuffers()) {
			buffer->setRealTime(realTime);
		}
		try {
			start();
		}
		catch (const exception& e) {
            printf("%s", e.what());
		}
	}
}

bool NonRealTimeAudioServer::isRealTime()
{
    return realTime;
}

void NonRealTimeAudioServer::start()
{
	if (isRunning())
		return;

	if (realTime) {
		auto lServer = server;
		lServer->start();
	}
	else {
		startNRT();
	}
}

void NonRealTimeAudioServer::startNRT()
{
	if (!client) {
		startASAP = true;
		return;
	}
	
    if (nrtThread.joinable())
        nrtThread.join();
	
    nrtThread = std::thread(&NonRealTimeAudioServer::static_nrts, this);
}

void NonRealTimeAudioServer::static_nrts(void * args)
{
	static_cast<NonRealTimeAudioServer*>(args)->run();
}

void NonRealTimeAudioServer::stop()
{
    if(!isRunning())
        return;

    if(realTime) {
		auto lServer = server;
        lServer->stop();
    } else if(isRunning_) {
        stopNRT();
    }
}

void NonRealTimeAudioServer::stopNRT()
{
    isRunning_ = false;
    
    if (nrtThread.joinable())
        nrtThread.join();
}

void NonRealTimeAudioServer::close()
{
	server->close();
    server = nullptr;
}

void NonRealTimeAudioServer::setClient(shared_ptr<AudioClient> client)
{
	server->setClient(me);
    this->client = client;
    if(startASAP) {
        startASAP = false;
        start();
    }
}

bool NonRealTimeAudioServer::isRunning()
{
	auto lServer = server;
    return realTime ? lServer->isRunning() : isRunning_;
}

void NonRealTimeAudioServer::work(const float** inputBuffer, float** outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount) {
	auto externalAudioServer = dynamic_pointer_cast<ExternalAudioServer>(server);
	if (externalAudioServer) {
		externalAudioServer->work(inputBuffer, outputBuffer, nFrames, inputChannelCount, outputChannelCount);
	}
}

void NonRealTimeAudioServer::work(const float* const* inputBuffer, float* const* outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount) {
	auto externalAudioServer = dynamic_pointer_cast<ExternalAudioServer>(server);
	if (externalAudioServer) {
		externalAudioServer->work(inputBuffer, outputBuffer, nFrames, inputChannelCount, outputChannelCount);
	}
}

void NonRealTimeAudioServer::work(float* inputBuffer, float* outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount) {
	auto externalAudioServer = dynamic_pointer_cast<ExternalAudioServer>(server);
	if (externalAudioServer) {
		externalAudioServer->work(inputBuffer, outputBuffer, nFrames, inputChannelCount, outputChannelCount);
	}
}

void NonRealTimeAudioServer::work(int nFrames)
{
	auto lClient = client;
    if (lClient) lClient->work(nFrames);
}

void NonRealTimeAudioServer::run()
{
	isRunning_ = true;
    while (isRunning_) {
		work(server->getBufferSize());
    }
}

void NonRealTimeAudioServer::removeAudioBuffer(mpc::engine::audio::core::AudioBuffer* buffer) {
	auto lServer = server;
	lServer->removeAudioBuffer(buffer);
}

mpc::engine::audio::core::AudioBuffer* NonRealTimeAudioServer::createAudioBuffer(string name)
{
	auto lServer = server;
	auto buffer = lServer->createAudioBuffer(name);
	return buffer;
}

vector<string> NonRealTimeAudioServer::getAvailableOutputNames()
{
	auto lServer = server;
    return lServer->getAvailableOutputNames();
}

vector<string> NonRealTimeAudioServer::getAvailableInputNames()
{
	auto lServer = server;
    return lServer->getAvailableInputNames();
}

IOAudioProcess* NonRealTimeAudioServer::openAudioOutput(string name)
{
	auto lServer = server;
    return lServer->openAudioOutput(name);
}

IOAudioProcess* NonRealTimeAudioServer::openAudioInput(string name)
{
    return server->openAudioInput(name);
}

void NonRealTimeAudioServer::closeAudioOutput(IOAudioProcess* output)
{
    server->closeAudioOutput(output);
}

void NonRealTimeAudioServer::closeAudioInput(IOAudioProcess* input)
{
    server->closeAudioInput(input);
}

float NonRealTimeAudioServer::getSampleRate()
{
    return server->getSampleRate();
}

int NonRealTimeAudioServer::getInputLatencyFrames()
{
    return server->getInputLatencyFrames();
}

int NonRealTimeAudioServer::getOutputLatencyFrames()
{
    return server->getOutputLatencyFrames();
}

int NonRealTimeAudioServer::getTotalLatencyFrames()
{
    return server->getTotalLatencyFrames();
}

void NonRealTimeAudioServer::resizeBuffers(int newSize) {
	server->resizeBuffers(newSize);
}

NonRealTimeAudioServer::~NonRealTimeAudioServer() {
	if (nrtThread.joinable()) nrtThread.join();
}
