#include <engine/audio/server/AudioServerProcess.hpp>

#include <engine/audio/core/ChannelFormat.hpp>

using namespace mpc::engine::audio::server;
using namespace std;

AudioServerProcess::AudioServerProcess(string name)
{
	this->name = name;
	format = mpc::engine::audio::core::ChannelFormat::STEREO();
}

string AudioServerProcess::getName() {
	return name;
}

std::shared_ptr<mpc::engine::audio::core::ChannelFormat> AudioServerProcess::getChannelFormat() {
	return format;
}
