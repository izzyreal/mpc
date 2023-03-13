#include "MpcIndivFxMixerChannel.hpp"

using namespace ctoot::mpc;

MpcIndivFxMixerChannel::MpcIndivFxMixerChannel() 
{
	output = 0;
	volumeIndividualOut = 100;
	fxPath = 0;
	fxSendLevel = 0;
	followStereo = false;
}

void MpcIndivFxMixerChannel::setFollowStereo(bool b)
{
	if (followStereo == b) return;

	followStereo = b;
}

bool MpcIndivFxMixerChannel::isFollowingStereo()
{
    return followStereo;
}

int MpcIndivFxMixerChannel::getOutput()
{
    return output;
}

void MpcIndivFxMixerChannel::setOutput(int i)
{
	if (i < 0 || i > 8)
		return;

	output = i;
}

void MpcIndivFxMixerChannel::setVolumeIndividualOut(int i)
{
	if (i < 0 || i > 100)
		return;

	volumeIndividualOut = i;
}

int MpcIndivFxMixerChannel::getVolumeIndividualOut()
{
    return volumeIndividualOut;
}

void MpcIndivFxMixerChannel::setFxPath(int i)
{
	if (i < 0 || i > 4)
		return;

	fxPath = i;
}

int MpcIndivFxMixerChannel::getFxPath()
{
    return fxPath;
}

void MpcIndivFxMixerChannel::setFxSendLevel(int i)
{
	if (i < 0 || i > 100)
		return;

	fxSendLevel = i;
}

int MpcIndivFxMixerChannel::getFxSendLevel()
{
    return fxSendLevel;
}
