#include <engine/audio/server/AudioServerProcess.hpp>

using namespace mpc::engine::audio::server;
using namespace std;

AudioServerProcess::AudioServerProcess(string name)
{
	this->name = name;
}

string AudioServerProcess::getName() {
	return name;
}
