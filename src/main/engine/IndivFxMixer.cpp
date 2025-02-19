#include "IndivFxMixer.hpp"

#include <algorithm>

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
	output = std::clamp(i, 0, 8);
}

void IndivFxMixer::setVolumeIndividualOut(int i)
{
	volumeIndividualOut = std::clamp(i, 0, 100);
}

int IndivFxMixer::getVolumeIndividualOut()
{
    return volumeIndividualOut;
}

void IndivFxMixer::setFxPath(int i)
{
	fxPath = std::clamp(i, 0, 4);
}

int IndivFxMixer::getFxPath()
{
    return fxPath;
}

void IndivFxMixer::setFxSendLevel(int i)
{
	fxSendLevel = std::clamp(i, 0, 100);
}

int IndivFxMixer::getFxSendLevel()
{
    return fxSendLevel;
}
