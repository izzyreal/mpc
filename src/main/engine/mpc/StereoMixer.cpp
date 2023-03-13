#include "StereoMixer.hpp"

using namespace ctoot::mpc;
using namespace std;

StereoMixer::StereoMixer()
{
	panning = 50;
	level = 100;
}

void StereoMixer::setPanning(int i)
{
	if (i < 0 || i > 100)
		return;

	panning = i;
}

int StereoMixer::getPanning()
{
    return panning;
}

void StereoMixer::setLevel(int i)
{
	if (i < 0 || i > 100)
		return;

	level = i;
}

int StereoMixer::getLevel()
{
    return level;
}
