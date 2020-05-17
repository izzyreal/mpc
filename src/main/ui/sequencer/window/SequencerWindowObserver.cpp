#include "SequencerWindowObserver.hpp"

#include <Mpc.hpp>
#include <Util.hpp>

#include <lcdgui/FunctionKeys.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/NameGui.hpp>
#include <lcdgui/HorizontalBar.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::sequencer;
using namespace mpc::ui::sequencer::window;
using namespace std;

SequencerWindowObserver::SequencerWindowObserver()
{	
	nameGui = Mpc::instance().getUis().lock()->getNameGui();
	samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
	sequencer = Mpc::instance().getSequencer();
	sampler = Mpc::instance().getSampler();
	auto ls = Mpc::instance().getLayeredScreen().lock();
	csn = ls->getCurrentScreenName();
	fb = ls->getFunctionKeys();
	swGui = Mpc::instance().getUis().lock()->getSequencerWindowGui();
	auto lSequencer = sequencer.lock();
	seqNum = lSequencer->getActiveSequenceIndex();
	sequence = lSequencer->getSequence(seqNum);
	trackNum = lSequencer->getActiveTrackIndex();
	auto seq = sequence.lock();
	track = seq->getTrack(trackNum);

	auto lSampler = sampler.lock();
	int drum = track.lock()->getBusNumber() - 1;
	mpcSoundPlayerChannel = drum >= 0 ? lSampler->getDrum(drum) : nullptr;
	program = drum >= 0 ? dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock()) : weak_ptr<mpc::sampler::Program>();
	trackNameFirstLetterField = ls->lookupField("tracknamefirstletter");
	defaultTrackNameFirstLetterField = ls->lookupField("defaultnamefirstletter");
	trackNameRestLabel = ls->lookupLabel("tracknamerest");
	defaultTrackNameRestLabel = ls->lookupLabel("defaultnamerest");
	trField = ls->lookupField("tr");
	sqField = ls->lookupField("sq");
	tr0Field = ls->lookupField("tr0");
	tr1Field = ls->lookupField("tr1");
	sq0Field = ls->lookupField("sq0");
	sq1Field = ls->lookupField("sq1");
	
	inThisTrackField = ls->lookupField("inthistrack");
	
	swGui->addObserver(this);
	nameGui->addObserver(this);
	samplerGui->addObserver(this);
	lSequencer->addObserver(this);
	seq->addObserver(this);

	seq->getMetaTracks().at(2).lock()->addObserver(this);
	
	auto lTrk = track.lock();
	lTrk->addObserver(this);
	
	if (csn.compare("deletesequence") == 0)
	{
		displaySequenceNumberName();
	}
	else if (csn.compare("deletetrack") == 0)
	{
		displayTrackNumber();
	}
	else if (csn.compare("copysequence") == 0)
	{
		displaySequenceNumberNames();
	}
	else if (csn.compare("copytrack") == 0)
	{
		displayTrackNumberNames();
	}
}

void SequencerWindowObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	auto seq = sequence.lock();
	auto lTrk = track.lock();
	lTrk->deleteObserver(this);
	seq->deleteObserver(this);
	auto lSequencer = sequencer.lock();
	seqNum = lSequencer->getActiveSequenceIndex();
	seq = lSequencer->getSequence(seqNum).lock();
	trackNum = lSequencer->getActiveTrackIndex();
	track = seq->getTrack(trackNum);
	lTrk = track.lock();
	lTrk->addObserver(this);
	seq->addObserver(this);

	int yPos;

	if (s.compare("seqnumbername") == 0)
	{
		displaySequenceNumberName();
	}
	else if (s.compare("tracknumber") == 0)
	{
		displayTrackNumber();
	}
	else if (s.compare("tr0") == 0)
	{
		displayTrackNumberNames();
	}
	else if (s.compare("tr1") == 0)
	{
		displayTrackNumberNames();
	}
	else if (s.compare("sq0") == 0)
	{
		displaySequenceNumberNames();
	}
	else if (s.compare("sq1") == 0)
	{
		displaySequenceNumberNames();
	}
}

void SequencerWindowObserver::displayTrackNumber()
{
	trField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getTrackNumber() + 1), " ", 2) + "-" +
		sequence.lock()->getTrack(swGui->getTrackNumber()).lock()->getName());
}

void SequencerWindowObserver::displaySequenceNumberName()
{
	auto lSequencer = sequencer.lock();
	auto sequenceName = lSequencer->getActiveSequence().lock()->getName();
    sqField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), " ", 2) + "-" + sequenceName);
}

void SequencerWindowObserver::displaySequenceNumberNames()
{
	auto lSequencer = sequencer.lock();
	auto sq0 = lSequencer->getSequence(swGui->getSq0()).lock()->getName();
    sq0Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getSq0() + 1), " ", 2) + "-" + sq0);
    auto sq1 = lSequencer->getSequence(swGui->getSq1()).lock()->getName();
    sq1Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getSq1() + 1), " ", 2) + "-" + sq1);
}

void SequencerWindowObserver::displayTrackNumberNames()
{
	auto seq = sequence.lock();
    auto tr0 = seq->getTrack(swGui->getTr0()).lock()->getName();
	tr0Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getTr0() + 1), " ", 2) + "-" + tr0);
    auto tr1 = seq->getTrack(swGui->getTr1()).lock()->getName();
    tr1Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getTr1() + 1), " ", 2) + "-" + tr1);
}

SequencerWindowObserver::~SequencerWindowObserver() {

	if (track.lock())
	{
		track.lock()->deleteObserver(this);
	}

	sequencer.lock()->deleteObserver(this);
	swGui->deleteObserver(this);
	nameGui->deleteObserver(this);
	samplerGui->deleteObserver(this);

	if (sequence.lock())
	{
		sequence.lock()->deleteObserver(this);
		sequence.lock()->getMetaTracks().at(2).lock()->deleteObserver(this);
	}
}
