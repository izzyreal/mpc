#include <controls/mixer/MixerControls.hpp>

#include <ui/sampler/MixerGui.hpp>
#include <ui/sampler/MixerSetupGui.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>

#include <lcdgui/Background.hpp>

using namespace mpc::controls::mixer;
using namespace std;

MixerControls::MixerControls() 
	: AbstractMixerControls()
{
}

void MixerControls::pad(int i, int velo, bool repeat, int tick) {
	init();
	mixerGui->setXPos(i);
	super::pad(i, velo, repeat, tick);
}

void MixerControls::up()
{
    init();
	mixerGui->setYPos(mixerGui->getYPos() - 1);
}

void MixerControls::down()
{
    init();
	mixerGui->setYPos(mixerGui->getYPos() + 1);
}

void MixerControls::left()
{
    init();
	mixerGui->setXPos(mixerGui->getXPos() - 1);
}

void MixerControls::right()
{
    init();
	mixerGui->setXPos(mixerGui->getXPos() + 1);
}

void MixerControls::openWindow()
{
	init();
	int pad = mixerGui->getXPos() + (bank_ * 16);
	int note = program.lock()->getPad(pad)->getNote();
	samplerGui->setPadAndNote(pad, note);
	ls.lock()->openScreen("channelsettings");
}

void MixerControls::function(int f)
{
	init();
	switch (f) {
	case 0:
	case 1:
	case 2:
		mixerGui->setTab(f);
		ls.lock()->getCurrentBackground()->SetDirty();
		break;
	case 3:
		ls.lock()->openScreen("mixersetup");
		break;
	case 4:
		ls.lock()->openScreen("fxedit");
	case 5:
		mixerGui->setLink(!mixerGui->getLink());
		break;
	}
}

void MixerControls::turnWheel(int i)
{
	init();
	int pad = mixerGui->getXPos() + (bank_ * 16);
	auto lProgram = program.lock();

	bool sSrcDrum = mixerSetupGui->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupGui->isIndivFxSourceDrum();
	auto drum = Mpc::instance().getDrum(samplerGui->getSelectedDrum());

	auto smc = sSrcDrum ? drum->getStereoMixerChannels().at(pad).lock() : lProgram->getPad(pad)->getStereoMixerChannel().lock();
	auto smcs = sSrcDrum ? drum->getStereoMixerChannels() : vector<weak_ptr<ctoot::mpc::MpcStereoMixerChannel>>(16);
	auto ifmc = iSrcDrum ? drum->getIndivFxMixerChannels().at(pad).lock() : lProgram->getPad(pad)->getIndivFxMixerChannel().lock();
	auto ifmcs = iSrcDrum ? drum->getIndivFxMixerChannels() : vector<weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel>>(16);

	for (int i = 0; i < 16; i++) {
		if (!sSrcDrum) smcs[i] = lProgram->getPad(i + (bank_ * 16))->getStereoMixerChannel().lock();
		if (!iSrcDrum) ifmcs[i] = lProgram->getPad(i + (bank_ * 16))->getIndivFxMixerChannel().lock();
	}

	if (mixerGui->getTab() == 0) {
		bool record = sequencer.lock()->isRecordingOrOverdubbing() && mixerSetupGui->isRecordMixChangesEnabled();
		if (mixerGui->getYPos() == 0) {
			if (!mixerGui->getLink()) {
				smc->setPanning(smc->getPanning() + i);
				if (record) {
					recordMixerEvent(pad, 1, smc->getPanning());
				}
			}
			else {
				int padCounter = 0;
				for (auto mcTemp : smcs) {
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setPanning(lMcTemp->getPanning() + i);
					if (record) {
						recordMixerEvent(padCounter++, 1, lMcTemp->getPanning());
					}
				}
			}
		}
		else if (mixerGui->getYPos() == 1) {
			if (!mixerGui->getLink()) {
				smc->setLevel(smc->getLevel() + i);
				if (record) {
					recordMixerEvent(pad, 0, smc->getLevel());
				}
			}
			else {
				auto padCounter = 0;
				for (auto mcTemp : smcs) {
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setLevel(lMcTemp->getLevel() + i);
					if (record) {
						recordMixerEvent(padCounter++, 0, lMcTemp->getLevel());
					}
				}
			}
		}
	}
	else if (mixerGui->getTab() == 1) {
		if (mixerGui->getYPos() == 0) {
			if (!mixerGui->getLink()) {
				ifmc->setOutput(ifmc->getOutput() + i);
				return;
			}
			else {
				for (auto mcTemp : ifmcs) {
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setOutput(lMcTemp->getOutput() + i);
				}
			}
		}
		else if (mixerGui->getYPos() == 1) {
			if (!mixerGui->getLink()) {
				ifmc->setVolumeIndividualOut(ifmc->getVolumeIndividualOut() + i);
			}
			else {
				for (auto mcTemp : ifmcs) {
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setVolumeIndividualOut(lMcTemp->getVolumeIndividualOut() + i);
				}
			}
		}
	}
	else if (mixerGui->getTab() == 2) {
		if (mixerGui->getYPos() == 0) {
			if (!mixerGui->getLink()) {
				ifmc->setFxPath(ifmc->getFxPath() + i);
			}
			else {
				for (auto mcTemp : ifmcs) {
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setFxPath(lMcTemp->getFxPath() + i);
				}
			}
		}
		else if (mixerGui->getYPos() == 1) {
			if (!mixerGui->getLink()) {
				ifmc->setFxSendLevel(ifmc->getFxSendLevel() + i);
			}
			else {
				for (auto mcTemp : ifmcs) {
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setFxSendLevel(lMcTemp->getFxSendLevel() + i);
				}
			}
		}
	}
}

void MixerControls::recordMixerEvent(int pad, int param, int value)
{
	auto lSequencer = sequencer.lock();
	auto track = lSequencer->getActiveSequence().lock()->getTrack(lSequencer->getActiveTrackIndex()).lock();
	auto e = dynamic_pointer_cast<mpc::sequencer::MixerEvent>(track->addEvent(lSequencer->getTickPosition(), "mixer").lock());
	e->setPadNumber(pad);
	e->setParameter(param);
	e->setValue(value);
}

MixerControls::~MixerControls() {
}
