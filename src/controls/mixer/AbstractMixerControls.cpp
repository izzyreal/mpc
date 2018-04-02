#include <controls/mixer/AbstractMixerControls.hpp>

#include <Mpc.hpp>
#include <ui/sampler/MixerGui.hpp>
#include <ui/sampler/MixerSetupGui.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/StereoMixerChannel.hpp>
#include <sampler/IndivFxMixerChannel.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>

using namespace mpc::controls::mixer;
using namespace std;

AbstractMixerControls::AbstractMixerControls(mpc::Mpc* mpc) 
	: AbstractControls(mpc)
{
	mixerGui = mpc->getUis().lock()->getMixerGui();
	mixerSetupGui = mpc->getUis().lock()->getMixerSetupGui();
}

void AbstractMixerControls::init()
{
	super::init();
	auto lProgram = program.lock();
	stereoMixerChannel = lProgram->getPad(samplerGui->getPad())->getStereoMixerChannel();
	indivFxMixerChannel = lProgram->getPad(samplerGui->getPad())->getIndivFxMixerChannel();
}

AbstractMixerControls::~AbstractMixerControls() {
}
