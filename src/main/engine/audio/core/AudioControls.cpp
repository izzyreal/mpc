#include <engine/audio/core/AudioControls.hpp>

using namespace ctoot::audio::core;
using namespace std;

AudioControls::AudioControls(int id, string name) : AudioControls(id, name, 127) {
}

AudioControls::AudioControls(int id, string name, int bypassId) : CompoundControl(id, name)
{
}
