#pragma once
#include <engine/audio/core/AudioProcess.hpp>

namespace ctoot::audio::core {
class AudioProcessAdapter
: public AudioProcess
{
    
protected:
    AudioProcess* process;
    
public:
    int processAudio(ctoot::audio::core::AudioBuffer* buf, int nFrames) override;

    AudioProcessAdapter(AudioProcess* process);
    
};
}
