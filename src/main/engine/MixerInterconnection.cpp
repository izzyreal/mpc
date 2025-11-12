#include "MixerInterconnection.hpp"

#include "InterconnectionInputProcess.hpp"
#include "InterconnectionOutputProcess.hpp"

#include "engine/audio/core/AudioProcess.hpp"
#include "engine/audio/server/AudioServer.hpp"

using namespace mpc::engine::audio::core;

using namespace mpc::engine;
using namespace std;

MixerInterconnection::MixerInterconnection(const string &name,
                                           audio::server::AudioServer *server)
{
    auto const sharedBuffer = server->createAudioBuffer(name);
    inputProcess =
        make_shared<InterconnectionInputProcess>(this, sharedBuffer.get());
    outputProcess =
        make_shared<InterconnectionOutputProcess>(sharedBuffer.get());
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

bool MixerInterconnection::isLeftEnabled() const
{
    return leftEnabled;
}

bool MixerInterconnection::isRightEnabled() const
{
    return rightEnabled;
}
