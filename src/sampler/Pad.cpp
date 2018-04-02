#include <sampler/Pad.hpp>

#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/MixerSetupGui.hpp>
#include <StartUp.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::sampler;
using namespace std;

Pad::Pad(mpc::Mpc* mpc, int number)
{
	this->mpc = mpc;
	this->number = number;
	note = mpc::StartUp::getUserDefaults().lock()->getPadNotes()[number];
	stereoMixerChannel = make_shared<StereoMixerChannel>();
	indivFxMixerChannel = make_shared<IndivFxMixerChannel>();
}

void Pad::setNote(int i)
{
	if (i < 34 || i > 98) return;

	if (mpc->getUis().lock()->getSamplerGui()->isPadAssignMaster()) {
		(*mpc->getSampler().lock()->getMasterPadAssign())[number] = i;
	}
	else {
		note = i;
	}
	setChanged();
	notifyObservers(string("padnotenumber"));
	setChanged();
	notifyObservers(string("note"));
	setChanged();
	notifyObservers(string("samplenumber"));
}

int Pad::getNote()
{
	if (mpc->getUis().lock()->getSamplerGui()->isPadAssignMaster()) {
		return (*mpc->getSampler().lock()->getMasterPadAssign())[number];
	}
	return note;
}

weak_ptr<StereoMixerChannel> Pad::getStereoMixerChannel()
{
    return stereoMixerChannel;
}

weak_ptr<IndivFxMixerChannel> Pad::getIndivFxMixerChannel()
{
	return indivFxMixerChannel;
}

int Pad::getNumber()
{
    return number;
}

Pad::~Pad() {
}
