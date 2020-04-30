#include <sampler/Pad.hpp>

#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/MixerSetupGui.hpp>
#include <StartUp.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::sampler;
using namespace std;

Pad::Pad(int number)
{
	
	this->number = number;
	note = mpc::StartUp::getUserDefaults().lock()->getPadNotes()[number];
	stereoMixerChannel = make_shared<ctoot::mpc::MpcStereoMixerChannel>();
	indivFxMixerChannel = make_shared<ctoot::mpc::MpcIndivFxMixerChannel>();
}

void Pad::setNote(int i)
{
	if (i < 34 || i > 98) return;

	if (Mpc::instance().getUis().lock()->getSamplerGui()->isPadAssignMaster()) {
		(*Mpc::instance().getSampler().lock()->getMasterPadAssign())[number] = i;
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
	if (Mpc::instance().getUis().lock()->getSamplerGui()->isPadAssignMaster()) {
		return (*Mpc::instance().getSampler().lock()->getMasterPadAssign())[number];
	}
	return note;
}

weak_ptr<ctoot::mpc::MpcStereoMixerChannel> Pad::getStereoMixerChannel()
{
    return stereoMixerChannel;
}

weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel> Pad::getIndivFxMixerChannel()
{
	return indivFxMixerChannel;
}

int Pad::getNumber()
{
    return number;
}

Pad::~Pad() {
}
