#include <file/pgmwriter/Mixer.hpp>

#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>

using namespace mpc::file::pgmwriter;
using namespace mpc::sampler;

Mixer::Mixer(mpc::sampler::Program* program)
{
	mixerArray = std::vector<char>(384 + 3);

	for (int i = 0; i < 64; i++)
	{
		auto noteParameters = dynamic_cast<NoteParameters*>(program->getNoteParameters(i + 35));
		auto smc = noteParameters->getStereoMixerChannel();
		auto ifmc = noteParameters->getIndivFxMixerChannel();
		setVolume(i, smc->getLevel());
		setPan(i, smc->getPanning());
		setVolumeIndividual(i, ifmc->getVolumeIndividualOut());
		setOutput(i, ifmc->getOutput());
		setEffectsSendLevel(i, ifmc->getFxSendLevel());
		setEffectsOutput(i, ifmc->getFxPath());
	}

	mixerArray[384] = 0;
	mixerArray[385] = 64;
	mixerArray[386] = 0;
}

std::vector<char> Mixer::getMixerArray()
{
    return mixerArray;
}

void Mixer::setEffectsOutput(int pad, int effectsOutputNumber)
{
    mixerArray[(pad * 6) + 0] = (char)(effectsOutputNumber);
}

void Mixer::setVolume(int pad, int volume)
{
    mixerArray[(pad * 6) + 1] = (char)(volume);
}

void Mixer::setPan(int pad, int pan)
{
    mixerArray[(pad * 6) + 2] = (char)(pan);
}

void Mixer::setVolumeIndividual(int pad, int volumeIndividual)
{
    mixerArray[(pad * 6) + 3] = (char)(volumeIndividual);
}

void Mixer::setOutput(int pad, int output)
{
    mixerArray[(pad * 6) + 4] = (char)(output);
}

void Mixer::setEffectsSendLevel(int pad, int effectsSendLevel)
{
    mixerArray[(pad * 6) + 5] = (char)(effectsSendLevel);
}
