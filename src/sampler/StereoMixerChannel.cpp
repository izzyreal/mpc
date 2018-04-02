#include <sampler/StereoMixerChannel.hpp>

using namespace mpc::sampler;
using namespace std;

StereoMixerChannel::StereoMixerChannel() 
{
	panning = 50;
	level = 100;
	stereo = true;
}

void StereoMixerChannel::setStereo(bool b)
{
    stereo = b;
}

bool StereoMixerChannel::isStereo()
{
    return stereo;
}

void StereoMixerChannel::setPanning(int i)
{
	if (i < 0 || i > 100)
		return;

	panning = i;
	setChanged();
	notifyObservers(string("panning"));
}

int StereoMixerChannel::getPanning()
{
    return panning;
}

void StereoMixerChannel::setLevel(int i)
{
	if (i < 0 || i > 100)
		return;

	level = i;
	setChanged();
	notifyObservers(string("volume"));
}

int StereoMixerChannel::getLevel()
{
    return level;
}
