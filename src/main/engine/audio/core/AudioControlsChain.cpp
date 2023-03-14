#include "AudioControlsChain.hpp"

#include "AudioControls.hpp"

using namespace mpc::engine::audio::core;
using namespace mpc::engine::control;

AudioControlsChain::AudioControlsChain(int id, std::string name)
	: CompoundControlChain(id, name)
{
}

