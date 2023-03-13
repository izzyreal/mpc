#include "BusControls.hpp"

#include <engine/audio/core/ChannelFormat.hpp>

using namespace ctoot::audio::core;
using namespace ctoot::control;
using namespace ctoot::audio::mixer;
using namespace std;

BusControls::BusControls(int id, string name, shared_ptr<ChannelFormat> format)
	: AudioControls(id, name, -1)
{
	channelFormat = format;
}

shared_ptr<ChannelFormat> BusControls::getChannelFormat()
{
    return channelFormat;
}
