#include "BusControls.hpp"

#include <engine/audio/core/ChannelFormat.hpp>

using namespace mpc::engine::audio::core;
using namespace mpc::engine::control;
using namespace mpc::engine::audio::mixer;
using namespace std;

BusControls::BusControls(int id, string name, shared_ptr<ChannelFormat> format)
	: AudioControls(id, name)
{
	channelFormat = format;
}

shared_ptr<ChannelFormat> BusControls::getChannelFormat()
{
    return channelFormat;
}
