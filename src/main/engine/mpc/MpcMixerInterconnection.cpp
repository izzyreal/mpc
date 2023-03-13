#include "MpcMixerInterconnection.hpp"

#include "InterconnectionInputProcess.hpp"
#include "InterconnectionOutputProcess.hpp"

#include <engine/audio/core/AudioBuffer.hpp>
#include <engine/audio/core/AudioProcess.hpp>
#include <engine/audio/server/AudioServer.hpp>

using namespace ctoot::audio::core;

using namespace ctoot::mpc;
using namespace std;

MpcMixerInterconnection::MpcMixerInterconnection(string name, ctoot::audio::server::AudioServer* server)
{
	auto const sharedBuffer = server->createAudioBuffer(name);
	inputProcess = make_shared<InterconnectionInputProcess>(this, sharedBuffer);
	outputProcess = make_shared<InterconnectionOutputProcess>(sharedBuffer);
}

shared_ptr<AudioProcess> MpcMixerInterconnection::getInputProcess()
{
    return inputProcess;
}

shared_ptr<AudioProcess> MpcMixerInterconnection::getOutputProcess()
{
    return outputProcess;
}

void MpcMixerInterconnection::setLeftEnabled(bool b)
{
    leftEnabled = b;
}

void MpcMixerInterconnection::setRightEnabled(bool b)
{
    rightEnabled = b;
}

bool MpcMixerInterconnection::isLeftEnabled() {
	return leftEnabled;
}

bool MpcMixerInterconnection::isRightEnabled() {
	return rightEnabled;
}

MpcMixerInterconnection::~MpcMixerInterconnection() {
}
