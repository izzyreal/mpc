#include <controls/mixer/AbstractMixerControls.hpp>

#include <Mpc.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>

using namespace mpc::controls::mixer;
using namespace std;

AbstractMixerControls::AbstractMixerControls() 
	: BaseControls()
{
}

void AbstractMixerControls::init()
{
	super::init();
	auto lProgram = program.lock();
	stereoMixerChannel = lProgram->getPad(mpc.getPad())->getStereoMixerChannel();
	indivFxMixerChannel = lProgram->getPad(mpc.getPad())->getIndivFxMixerChannel();
}
