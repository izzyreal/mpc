#include <engine/audio/server/AudioServerProcess.hpp>

#include <engine/audio/core/ChannelFormat.hpp>

using namespace ctoot::audio::server;
using namespace std;

AudioServerProcess::AudioServerProcess(string name)
{
	this->name = name;
	format = ctoot::audio::core::ChannelFormat::STEREO();
}

string AudioServerProcess::getName() {
	return name;
}

std::shared_ptr<ctoot::audio::core::ChannelFormat> AudioServerProcess::getChannelFormat() {
	return format;
}
