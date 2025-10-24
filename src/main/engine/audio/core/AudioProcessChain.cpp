#include "AudioProcessChain.hpp"

#include "engine/audio/core/AudioControls.hpp"
#include "engine/audio/core/AudioControlsChain.hpp"
#include "engine/audio/core/AudioProcess.hpp"

using namespace std;
using namespace mpc::engine::control;
using namespace mpc::engine::audio::core;

AudioProcessChain::AudioProcessChain(shared_ptr<AudioControlsChain> controlChain)
{
	this->controlChain = controlChain;
}

void AudioProcessChain::open()
{
	for (auto& control : controlChain->getControls()) {

		auto candidate = dynamic_pointer_cast<AudioControls>(control);

		if (candidate) {
			auto p = createProcess(candidate);
			if (p != nullptr) {
                processes.push_back(p);
				p->open();
			}
		}
	}
}

int AudioProcessChain::processAudio(AudioBuffer* buffer, int nFrames)
{
	for (auto& p : processes)
    {
        p->processAudio(buffer, nFrames);
	}

	return AUDIO_OK;
}

void AudioProcessChain::close()
{
	for (auto& p : processes) {
		if (!p) continue;
		p->close();
	}
	processes.clear();
}

string AudioProcessChain::getName()
{
    return controlChain->getName();
}
