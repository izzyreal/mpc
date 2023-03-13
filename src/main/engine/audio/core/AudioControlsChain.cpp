#include "AudioControlsChain.hpp"

#include "AudioControls.hpp"

using namespace ctoot::audio::core;
using namespace ctoot::control;

AudioControlsChain::AudioControlsChain(int id, std::string name)
	: CompoundControlChain(id, name)
{
}

