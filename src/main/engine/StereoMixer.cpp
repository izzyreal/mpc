#include "StereoMixer.hpp"

#include <algorithm>

using namespace mpc::engine;

StereoMixer::StereoMixer()
{
    panning = 50;
    level = 100;
}

void StereoMixer::setPanning(int i)
{
    panning = std::clamp(i, 0, 100);
}

int StereoMixer::getPanning() const
{
    return panning;
}

void StereoMixer::setLevel(int i)
{
    level = std::clamp(i, 0, 100);
}

int StereoMixer::getLevel() const
{
    return level;
}
