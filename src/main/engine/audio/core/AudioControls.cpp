#include <engine/audio/core/AudioControls.hpp>

using namespace mpc::engine::audio::core;
using namespace std;

AudioControls::AudioControls(int id, string name) : AudioControls(id, name, 127) {
}

AudioControls::AudioControls(int id, string name, int bypassId) : CompoundControl(id, name)
{
}
