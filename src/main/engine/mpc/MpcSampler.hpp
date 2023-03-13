#pragma once

#include <memory>

namespace ctoot::mpc
{
class MpcSound;
class MpcProgram;

class MpcSampler {
    
public:
    virtual std::shared_ptr<MpcSound> getMpcPreviewSound() = 0;
    virtual std::shared_ptr<MpcSound> getPlayXSound() = 0;
    virtual std::shared_ptr<MpcSound> getClickSound() = 0;
    virtual std::shared_ptr<MpcSound> getMpcSound(int soundNumber) = 0;
    virtual std::shared_ptr<MpcProgram> getMpcProgram(int i) = 0;
    
};
}
