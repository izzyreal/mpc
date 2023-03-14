#include "IndivFxMixer.hpp"

using namespace mpc::engine;

IndivFxMixer::IndivFxMixer()
{
	output = 0;
	volumeIndividualOut = 100;
	fxPath = 0;
	fxSendLevel = 0;
	followStereo = false;
}

void IndivFxMixer::setFollowStereo(bool b)
{
	if (followStereo == b) return;

	followStereo = b;
}

bool IndivFxMixer::isFollowingStereo()
{
    return followStereo;
}

int IndivFxMixer::getOutput()
{
    return output;
}

void IndivFxMixer::setOutput(int i)
{
	if (i < 0 || i > 8)
		return;

	output = i;
}

void IndivFxMixer::setVolumeIndividualOut(int i)
{
	if (i < 0 || i > 100)
		return;

	volumeIndividualOut = i;
}

int IndivFxMixer::getVolumeIndividualOut()
{
    return volumeIndividualOut;
}

void IndivFxMixer::setFxPath(int i)
{
	if (i < 0 || i > 4)
		return;

	fxPath = i;
}

int IndivFxMixer::getFxPath()
{
    return fxPath;
}

void IndivFxMixer::setFxSendLevel(int i)
{
	if (i < 0 || i > 100)
		return;

	fxSendLevel = i;
}

int IndivFxMixer::getFxSendLevel()
{
    return fxSendLevel;
}
