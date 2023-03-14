#include "MixerInterconnection.hpp"

#include "InterconnectionInputProcess.hpp"
#include "InterconnectionOutputProcess.hpp"

#include <engine/audio/core/AudioBuffer.hpp>
#include <engine/audio/core/AudioProcess.hpp>
#include <engine/audio/server/AudioServer.hpp>

using namespace ctoot::audio::core;

using namespace ctoot::mpc;
using namespace std;

MixerInterconnection::MixerInterconnection(string name, ctoot::audio::server::AudioServer* server)
{
	auto const sharedBuffer = server->createAudioBuffer(name);
	inputProcess = make_shared<InterconnectionInputProcess>(this, sharedBuffer);
	outputProcess = make_shared<InterconnectionOutputProcess>(sharedBuffer);
}

shared_ptr<AudioProcess> MixerInterconnection::getInputProcess()
{
    return inputProcess;
}

shared_ptr<AudioProcess> MixerInterconnection::getOutputProcess()
{
    return outputProcess;
}

void MixerInterconnection::setLeftEnabled(bool b)
{
    leftEnabled = b;
}

void MixerInterconnection::setRightEnabled(bool b)
{
    rightEnabled = b;
}

bool MixerInterconnection::isLeftEnabled() {
	return leftEnabled;
}

bool MixerInterconnection::isRightEnabled() {
	return rightEnabled;
}

MixerInterconnection::~MixerInterconnection() {
}
