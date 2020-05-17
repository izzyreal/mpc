#include "AllLoader.hpp"

#include <Mpc.hpp>
#include <file/all/Bar.hpp>
#include <file/all/BarList.hpp>
#include <file/all/Count.hpp>
#include <file/all/Defaults.hpp>
#include <file/all/MidiInput.hpp>
#include <file/all/MidiSyncMisc.hpp>
#include <file/all/Misc.hpp>
#include <file/all/AllSequence.hpp>
#include <file/all/SequenceNames.hpp>
#include <file/all/AllSequencer.hpp>
#include <file/all/AllSong.hpp>
#include <file/all/Tracks.hpp>

#include <ui/UserDefaults.hpp>
#include <ui/midisync/MidiSyncGui.hpp>
#include <ui/sequencer/SongGui.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Song.hpp>
#include <sequencer/TempoChangeEvent.hpp>

#include <lcdgui/Screens.hpp>

#include <lcdgui/screens/window/CountMetronomeScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/MultiRecordingSetupScreen.hpp>
#include <lcdgui/screens/window/MidiInputScreen.hpp>
#include <lcdgui/screens/window/MultiRecordingSetupLine.hpp>

#include <lcdgui/screens/dialog/MetronomeSoundScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::disk;
using namespace mpc::file::all;
using namespace std;

AllLoader::AllLoader(mpc::disk::MpcFile* file, bool sequencesOnly)
	: allParser(AllParser(file))
{
	if (sequencesOnly)
	{
		allSequences = allParser.getAllSequences();
		auto allSeqNames = allParser.getSeqNames()->getNames();
		vector<Sequence*> temp;
		int counter = 0;
		
		for (int i = 0; i < 99; i++)
		{
			if (allSeqNames.at(i).find("(Unused)") != string::npos)
			{
				temp.push_back(nullptr);
			}
			else
			{
				temp.push_back(allSequences[counter++]);
			}
		}
		allSequences = temp;
		convertSequences(true);
	}
	else {
		auto lSequencer = Mpc::instance().getSequencer().lock();
		allSequences = allParser.getAllSequences();
		auto defaults = allParser.getDefaults();
		
		auto& ud = mpc::ui::UserDefaults::instance();
		
		ud.setLastBar(defaults->getBarCount() - 1);
		ud.setBus(defaults->getBusses()[0]);
		
		for (int i = 0; i < 33; i++) {
			ud.setDeviceName(i, defaults->getDefaultDevNames()[i]);
		}

		ud.setSequenceName(defaults->getDefaultSeqName());
		auto defTrackNames = defaults->getDefaultTrackNames();
		
		for (int i = 0; i < 64; i++) {
			ud.setTrackName(i, defTrackNames[i]);
		}

		ud.setDeviceNumber(defaults->getDevices()[0]);
		ud.setTimeSig(defaults->getTimeSigNum(), defaults->getTimeSigDen());
		ud.setPgm(defaults->getPgms()[0]);
		ud.setTempo(BCMath(defaults->getTempo() / 10.0));
		ud.setVelo(defaults->getTrVelos()[0]);
		convertSequences(false);
		auto allSeqNames = allParser.getSeqNames()->getNames();
		auto sequencer = allParser.getSequencer();
		lSequencer->setActiveSequenceIndex(sequencer->sequence);
		lSequencer->setSelectedTrackIndex(sequencer->track);
		
		auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timingcorrect"));
		
		timingCorrectScreen->setNoteValue(sequencer->tc);
		
		auto swGui = Mpc::instance().getUis().lock()->getSequencerWindowGui();
		auto count = allParser.getCount();

		auto countMetronomeScreen = dynamic_pointer_cast<CountMetronomeScreen>(Screens::getScreenComponent("countmetronome"));
		auto metronomeSoundScreen = dynamic_pointer_cast<MetronomeSoundScreen>(Screens::getScreenComponent("metronomesound"));

		countMetronomeScreen->setCountIn(count->getCountInMode());
		metronomeSoundScreen->setAccentVelo(count->getAccentVelo());
		metronomeSoundScreen->setNormalVelo(count->getNormalVelo());
		metronomeSoundScreen->setClickOutput(count->getClickOutput());
		metronomeSoundScreen->setClickVolume(count->getClickVolume());
		countMetronomeScreen->setRate(count->getRate());
		metronomeSoundScreen->setMetronomeSound(count->getSound());
		countMetronomeScreen->setInPlay(count->isEnabledInPlay());
		countMetronomeScreen->setInRec(count->isEnabledInRec());
		countMetronomeScreen->setWaitForKey(count->isWaitForKeyEnabled());
		lSequencer->setCountEnabled(count->isEnabled());

		auto midiInput = allParser.getMidiInput();
		auto midiInputScreen = dynamic_pointer_cast<MidiInputScreen>(Screens::getScreenComponent("midiinput"));

		midiInputScreen->setReceiveCh(midiInput->getReceiveCh());
		midiInputScreen->setType(midiInput->getFilterType());
		midiInputScreen->setMidiFilterEnabled(midiInput->isFilterEnabled());
		midiInputScreen->setSustainPedalToDuration(midiInput->isSustainPedalToDurationEnabled());

		auto trackDests = midiInput->getMultiRecTrackDests();
		
		auto multiRecordingSetupScreen = dynamic_pointer_cast<MultiRecordingSetupScreen>(Screens::getScreenComponent("multirecordingsetup"));

		for (int i = 0; i < trackDests.size(); i++) {
			multiRecordingSetupScreen->getMrsLines()[i]->setTrack(trackDests[i]);
		}

		midiInputScreen->setChPressurePassEnabled(midiInput->isChPressurePassEnabled());
		midiInputScreen->setExclusivePassEnabled(midiInput->isExclusivePassEnabled());
		lSequencer->setRecordingModeMulti(midiInput->isMultiRecEnabled());
		midiInputScreen->setNotePassEnabled(midiInput->isNotePassEnabled());
		midiInputScreen->setPgmChangePassEnabled(midiInput->isPgmChangePassEnabled());
		midiInputScreen->setPitchBendPassEnabled(midiInput->isPitchBendPassEnabled());
		midiInputScreen->setPolyPressurePassEnabled(midiInput->isPolyPressurePassEnabled());
		
		auto midiSyncMisc = allParser.getMidiSync();
		
		auto seGui = Mpc::instance().getUis().lock()->getStepEditorGui();
		
		auto misc = allParser.getMisc();
		seGui->setAutoStepIncrementEnabled(misc->isAutoStepIncEnabled());
		seGui->setTcValueRecordedNotes(misc->getDurationTcPercentage());
		seGui->setDurationOfRecordedNotes(misc->isDurationOfRecNotesTc());
		swGui->setTapAvg(misc->getTapAvg());

		auto msGui = Mpc::instance().getUis().lock()->getMidiSyncGui();
		msGui->setReceiveMMCEnabled(misc->isInReceiveMMCEnabled());
		msGui->setSendMMCEnabled(midiSyncMisc->isSendMMCEnabled());
		msGui->setModeIn(midiSyncMisc->getInMode());
		msGui->setModeOut(midiSyncMisc->getOutMode());
		msGui->setShiftEarly(midiSyncMisc->getShiftEarly());
		msGui->setFrameRate(midiSyncMisc->getFrameRate());
		msGui->setIn(midiSyncMisc->getInput());
		msGui->setOut(midiSyncMisc->getOutput());
		
		lSequencer->setSecondSequenceEnabled(sequencer->secondSeqEnabled);
		lSequencer->setSecondSequenceIndex(sequencer->secondSeqIndex);
		
		Mpc::instance().getUis().lock()->getSongGui()->setDefaultSongName(midiSyncMisc->getDefSongName());
				
		auto songs = allParser.getSongs();
		
		for (int i = 0; i < 20; i++)
		{
			lSequencer->getSong(i).lock()->setName(songs[i]->name);
		}

	}
}

void AllLoader::convertSequences(const bool indiv)
{
	int index = -1;
	
	if (!indiv)
	{
		Mpc::instance().getSequencer().lock()->purgeAllSequences();
	}

	for (auto& as : allSequences)
	{
		index++;
	
		if (as == nullptr)
		{
			if (indiv) mpcSequences.push_back(nullptr);
			continue;
		}

		shared_ptr<mpc::sequencer::Sequence> mpcSeq;
		
		if (indiv)
		{
			mpcSeq = make_shared<mpc::sequencer::Sequence>(Mpc::instance().getSequencer().lock()->getDefaultTrackNames());
		}
		else {
			mpcSeq = Mpc::instance().getSequencer().lock()->getSequence(index).lock();
		}
		
		mpcSeq->init(as->barCount - 1);
		
		for (int i = 0; i < as->barCount; i++)
		{
			auto num = dynamic_cast<Bar*>(as->barList->getBars()[i])->getNumerator();
			auto den = dynamic_cast<Bar*>(as->barList->getBars()[i])->getDenominator();
			mpcSeq->setTimeSignature(i, num, den);
		}

		mpcSeq->setName(as->name);
		mpcSeq->setInitialTempo(as->tempo);
		auto at = as->tracks;
		
		for (int i = 0; i < 64; i++)
		{
			auto t = mpcSeq->getTrack(i).lock();
			t->setUsed(at->getStatus(i) != 6);
			t->setName(at->getName(i));
			t->setBusNumber(at->getBus(i));
			t->setProgramChange(at->getPgm(i));
			t->setOn(at->getStatus(i) != 5);
			t->setVelocityRatio(at->getVelo(i));
		}

		for (int j = 0; j < as->getEventAmount(); j++)
		{
			auto e = as->allEvents[j];
			if (e == nullptr) continue;
			int track = e->getTrack();
			if (track > 128) track -= 128;
			if (track < 0) track += 128;
			if (track > 63) track -= 64;
			mpcSeq->getTrack(track).lock()->cloneEvent(shared_ptr<mpc::sequencer::Event>(e));
		}

		for (int i = 0; i < 32; i++)
		{
			mpcSeq->setDeviceName(i, as->devNames[i]);
		}

		mpcSeq->initMetaTracks();
		mpcSeq->setFirstLoopBar(as->loopFirst);
		mpcSeq->setLastLoopBar(as->loopLast);
		mpcSeq->setLastLoopBar(as->loopLast);
		
		if (as->loopLastEnd)
		{
			mpcSeq->setLastLoopBar(INT_MAX);
		}

		mpcSeq->setLoopEnabled(as->loop);
		
		if (indiv)
		{
			mpcSequences.push_back(mpcSeq);
		}
	}
}

vector<shared_ptr<mpc::sequencer::Sequence>>& AllLoader::getSequences()
{
    return mpcSequences;
}
