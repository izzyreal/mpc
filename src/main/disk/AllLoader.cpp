#include "AllLoader.hpp"

#include <Mpc.hpp>
#include <file/all/AllParser.hpp>
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
#include <file/all/Header.hpp>

#include <disk/MpcFile.hpp>
#include <file/FsNode.hpp>

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
#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/screens/SecondSeqScreen.hpp>
#include <lcdgui/screens/SongScreen.hpp>
#include <lcdgui/screens/OthersScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/UserScreen.hpp>

#include <lcdgui/screens/dialog/MetronomeSoundScreen.hpp>

#include <stdexcept>

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::disk;
using namespace mpc::file::all;
using namespace std;

AllLoader::AllLoader(mpc::Mpc& _mpc, mpc::disk::MpcFile* file, bool sequencesOnly)
    : mpc (_mpc)
{
    auto allParser = AllParser(_mpc, file);
    mpcSequences = AllLoader(_mpc, allParser, sequencesOnly).mpcSequences;
}

AllLoader::AllLoader(mpc::Mpc& _mpc, AllParser& allParser, bool sequencesOnly)
: mpc (_mpc)
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
    else
    {
        auto lSequencer = mpc.getSequencer().lock();
        allSequences = allParser.getAllSequences();
        auto defaults = allParser.getDefaults();
        
        auto userScreen = mpc.screens->get<UserScreen>("user");

        userScreen->setLastBar(defaults->getBarCount() - 1);
        userScreen->setBus(defaults->getBusses()[0]);
        
        for (int i = 0; i < 33; i++)
        {
            userScreen->setDeviceName(i, defaults->getDefaultDevNames()[i]);
        }

        userScreen->setSequenceName(defaults->getDefaultSeqName());
        auto defTrackNames = defaults->getDefaultTrackNames();
        
        for (int i = 0; i < 64; i++)
        {
            userScreen->setTrackName(i, defTrackNames[i]);
        }

        userScreen->setDeviceNumber(defaults->getDevices()[0]);
        userScreen->setTimeSig(defaults->getTimeSigNum(), defaults->getTimeSigDen());
        userScreen->setPgm(defaults->getPgms()[0]);
        userScreen->setTempo(defaults->getTempo() * 0.1);
        userScreen->setVelo(defaults->getTrVelos()[0]);

        convertSequences(false);
        
        auto allSeqNames = allParser.getSeqNames()->getNames();
        auto sequencer = allParser.getSequencer();
        lSequencer->setActiveSequenceIndex(sequencer->sequence);
        lSequencer->setActiveTrackIndex(sequencer->track);
        
        auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
        
        timingCorrectScreen->setNoteValue(sequencer->tc);
        
        auto count = allParser.getCount();

        auto countMetronomeScreen = mpc.screens->get<CountMetronomeScreen>("count-metronome");
        auto metronomeSoundScreen = mpc.screens->get<MetronomeSoundScreen>("metronome-sound");

        countMetronomeScreen->setCountIn(count->getCountInMode());
        metronomeSoundScreen->setAccentVelo(count->getAccentVelo());
        metronomeSoundScreen->setNormalVelo(count->getNormalVelo());
        metronomeSoundScreen->setOutput(count->getClickOutput());
        metronomeSoundScreen->setVolume(count->getClickVolume());
        countMetronomeScreen->setRate(count->getRate());
        metronomeSoundScreen->setSound(count->getSound());
        countMetronomeScreen->setInPlay(count->isEnabledInPlay());
        countMetronomeScreen->setInRec(count->isEnabledInRec());
        countMetronomeScreen->setWaitForKey(count->isWaitForKeyEnabled());
        lSequencer->setCountEnabled(count->isEnabled());

        auto midiInput = allParser.getMidiInput();
        auto midiInputScreen = mpc.screens->get<MidiInputScreen>("midi-input");

        midiInputScreen->setReceiveCh(midiInput->getReceiveCh());
        midiInputScreen->setType(midiInput->getFilterType());
        midiInputScreen->setMidiFilterEnabled(midiInput->isFilterEnabled());
        midiInputScreen->setSustainPedalToDuration(midiInput->isSustainPedalToDurationEnabled());

        auto trackDests = midiInput->getMultiRecTrackDests();
        
        auto multiRecordingSetupScreen = mpc.screens->get<MultiRecordingSetupScreen>("multi-recording-setup");

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
                
        auto misc = allParser.getMisc();
        auto stepEditorScreen = mpc.screens->get<StepEditorScreen>("step-editor");
        stepEditorScreen->setAutoStepIncrementEnabled(misc->isAutoStepIncEnabled());
        stepEditorScreen->setTcValueRecordedNotes(misc->getDurationTcPercentage());
        stepEditorScreen->setDurationOfRecordedNotes(misc->isDurationOfRecNotesTc());

        auto othersScreen = mpc.screens->get<OthersScreen>("others");

        othersScreen->setTapAveraging(misc->getTapAvg());

        auto syncScreen = mpc.screens->get<SyncScreen>("sync");

        syncScreen->receiveMMCEnabled = misc->isInReceiveMMCEnabled();
        syncScreen->sendMMCEnabled = midiSyncMisc->isSendMMCEnabled();
        syncScreen->in = midiSyncMisc->getInput();
        syncScreen->out = midiSyncMisc->getOutput();
        syncScreen->setModeIn(midiSyncMisc->getInMode());
        syncScreen->setModeOut(midiSyncMisc->getOutMode());
        syncScreen->shiftEarly = midiSyncMisc->getShiftEarly();
        syncScreen->frameRate = midiSyncMisc->getFrameRate();
        
        lSequencer->setSecondSequenceEnabled(sequencer->secondSeqEnabled);

        auto secondSequenceScreen = mpc.screens->get<SecondSeqScreen>("second-seq");
        secondSequenceScreen->sq = sequencer->secondSeqIndex;
        
        auto songScreen = mpc.screens->get<SongScreen>("song");
        songScreen->setDefaultSongName(midiSyncMisc->getDefSongName());
                
        auto songs = allParser.getSongs();
        
        for (int i = 0; i < 20; i++)
            lSequencer->getSong(i).lock()->setName(songs[i]->name);
    }
}

void AllLoader::convertSequences(const bool indiv)
{
	int index = -1;
	
	if (!indiv)
		mpc.getSequencer().lock()->purgeAllSequences();

	for (auto& as : allSequences)
	{
		index++;
	
		if (as == nullptr)
		{
			if (indiv)
				mpcSequences.push_back(nullptr);
			
			continue;
		}

		shared_ptr<mpc::sequencer::Sequence> mpcSeq;
		
		if (indiv)
		{
			mpcSeq = make_shared<mpc::sequencer::Sequence>(mpc, mpc.getSequencer().lock()->getDefaultTrackNames());
		}
		else {
			mpcSeq = mpc.getSequencer().lock()->getSequence(index).lock();
		}
		
		mpcSeq->init(as->barCount - 1);
		
		for (int i = 0; i < as->barCount; i++)
		{
			auto num = as->barList->getBars()[i]->getNumerator();
			auto den = as->barList->getBars()[i]->getDenominator();
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
			mpcSeq->setDeviceName(i, as->devNames[i]);

		mpcSeq->initMetaTracks();
		mpcSeq->setFirstLoopBarIndex(as->loopFirst);
		mpcSeq->setLastLoopBarIndex(as->loopLast);
		mpcSeq->setLastLoopBarIndex(as->loopLast);
		
		if (as->loopLastEnd)
			mpcSeq->setLastLoopBarIndex(INT_MAX);

		mpcSeq->setLoopEnabled(as->loop);
		
		if (indiv)
			mpcSequences.push_back(mpcSeq);
	}
}

vector<shared_ptr<mpc::sequencer::Sequence>>& AllLoader::getSequences()
{
    return mpcSequences;
}
