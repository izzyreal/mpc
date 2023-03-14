#pragma once

#include <memory>

namespace mpc::engine {

class MpcNoteParameters;
class StereoMixer;
class IndivFxMixer;

class MpcProgram {
    
public:
    virtual MpcNoteParameters* getNoteParameters(int i) = 0;
    virtual std::shared_ptr<StereoMixer> getStereoMixerChannel(int pad) = 0;
    virtual std::shared_ptr<IndivFxMixer> getIndivFxMixerChannel(int pad) = 0;
    
};
}
