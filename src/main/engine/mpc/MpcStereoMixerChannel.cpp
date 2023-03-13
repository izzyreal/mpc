#include "MpcStereoMixerChannel.hpp"

using namespace ctoot::mpc;
using namespace std;

MpcStereoMixerChannel::MpcStereoMixerChannel() 
{
	panning = 50;
	level = 100;
}

void MpcStereoMixerChannel::setPanning(int i)
{
	if (i < 0 || i > 100)
		return;

	panning = i;
}

int MpcStereoMixerChannel::getPanning()
{
    return panning;
}

void MpcStereoMixerChannel::setLevel(int i)
{
	if (i < 0 || i > 100)
		return;

	level = i;
}

int MpcStereoMixerChannel::getLevel()
{
    return level;
}
