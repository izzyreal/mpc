#include <sampler/IndivFxMixerChannel.hpp>

using namespace mpc::sampler;
using namespace std;

IndivFxMixerChannel::IndivFxMixerChannel() 
{
	output = 0;
	volumeIndividualOut = 100;
	fxPath = 0;
	fxSendLevel = 0;
	followStereo = false;
}

void IndivFxMixerChannel::setFollowStereo(bool b)
{
	if (followStereo == b) return;

	followStereo = b;
	setChanged();
	notifyObservers(string("followstereo"));
}

bool IndivFxMixerChannel::isFollowingStereo()
{
    return followStereo;
}

int IndivFxMixerChannel::getOutput()
{
    return output;
}

void IndivFxMixerChannel::setOutput(int i)
{
	if (i < 0 || i > 8)
		return;

	output = i;
	setChanged();
	notifyObservers(string("output"));
}

void IndivFxMixerChannel::setVolumeIndividualOut(int i)
{
	if (i < 0 || i > 100)
		return;

	volumeIndividualOut = i;
	setChanged();
	notifyObservers(string("volumeindividual"));
}

int IndivFxMixerChannel::getVolumeIndividualOut()
{
    return volumeIndividualOut;
}

void IndivFxMixerChannel::setFxPath(int i)
{
	if (i < 0 || i > 4)
		return;

	fxPath = i;
	setChanged();
	notifyObservers(string("fxpath"));
}

int IndivFxMixerChannel::getFxPath()
{
    return fxPath;
}

void IndivFxMixerChannel::setFxSendLevel(int i)
{
	if (i < 0 || i > 100)
		return;

	fxSendLevel = i;
	setChanged();
	notifyObservers(string("fxsendlevel"));
}

int IndivFxMixerChannel::getFxSendLevel()
{
    return fxSendLevel;
}
