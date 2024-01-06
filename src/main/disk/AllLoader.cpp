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

#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Song.hpp>
#include <sequencer/Step.hpp>
#include <sequencer/TempoChangeEvent.hpp>

#include <lcdgui/screens/window/CountMetronomeScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/MultiRecordingSetupScreen.hpp>
#include <lcdgui/screens/window/MidiInputScreen.hpp>
#include <lcdgui/screens/window/MidiOutputScreen.hpp>
#include <lcdgui/screens/window/StepEditOptionsScreen.hpp>
#include "lcdgui/screens/window/TimeDisplayScreen.hpp"
#include <lcdgui/screens/SecondSeqScreen.hpp>
#include <lcdgui/screens/SongScreen.hpp>
#include <lcdgui/screens/OthersScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/MidiSwScreen.hpp>

#include <lcdgui/screens/dialog/MetronomeSoundScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::disk;
using namespace mpc::file::all;
using namespace mpc::sequencer;

void AllLoader::loadEverythingFromFile(mpc::Mpc& mpc, mpc::disk::MpcFile* f)
{
    AllParser allParser(mpc, f->getBytes());
    AllLoader::loadEverythingFromAllParser(mpc, allParser);
}

void AllLoader::loadEverythingFromAllParser(mpc::Mpc& mpc, AllParser& allParser)
{
    auto mpcSequencer = mpc.getSequencer();
    auto allSequences = allParser.getAllSequences();
    auto allSeqNames = allParser.getSeqNames()->getNames();
    auto defaults = allParser.getDefaults();

    auto userScreen = mpc.screens->get<UserScreen>("user");

    userScreen->setLastBar(defaults->getBarCount() - 1);
    userScreen->setBus(defaults->getBusses()[0]);

    for (int i = 0; i < 33; i++)
        userScreen->setDeviceName(i, defaults->getDefaultDevNames()[i]);

    userScreen->setSequenceName(defaults->getDefaultSeqName());
    auto defTrackNames = defaults->getDefaultTrackNames();

    for (int i = 0; i < 64; i++)
        userScreen->setTrackName(i, defTrackNames[i]);

    userScreen->setDeviceNumber(defaults->getDevices()[0]);
    userScreen->setTimeSig(defaults->getTimeSigNum(), defaults->getTimeSigDen());
    userScreen->setPgm(defaults->getPgms()[0]);
    userScreen->setTempo(defaults->getTempo() * 0.1);
    userScreen->setVelo(defaults->getTrVelos()[0]);

    int index = 0;

    mpc.getSequencer()->purgeAllSequences();

    for (auto& as : allSequences)
    {
        if (as == nullptr)
        {
            index++;
            continue;
        }

        auto mpcSeq = mpc.getSequencer()->getSequence(index++);
        as->applyToMpcSeq(mpcSeq);
    }

    auto allParserSequencer = allParser.getSequencer();
    mpcSequencer->setActiveSequenceIndex(allParserSequencer->sequence);
    mpcSequencer->setActiveTrackIndex(allParserSequencer->track);
    mpcSequencer->setTempo(allParserSequencer->masterTempo);
    mpcSequencer->setTempoSourceSequence(allParserSequencer->tempoSourceIsSequence);

    auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
    timingCorrectScreen->setNoteValue(allParserSequencer->tc);

    auto timeDisplayScreen = mpc.screens->get<TimeDisplayScreen>("time-display");
    timeDisplayScreen->setDisplayStyle(allParserSequencer->timeDisplayStyle);

    auto count = allParser.getCount();

    auto countMetronomeScreen = mpc.screens->get<CountMetronomeScreen>("count-metronome");
    auto metronomeSoundScreen = mpc.screens->get<MetronomeSoundScreen>("metronome-sound");

    countMetronomeScreen->setCountIn(count->getCountInMode());
    metronomeSoundScreen->accentPad = count->getAccentPad();
    metronomeSoundScreen->normalPad = count->getNormalPad();
    metronomeSoundScreen->setAccentVelo(count->getAccentVelo());
    metronomeSoundScreen->setNormalVelo(count->getNormalVelo());
    metronomeSoundScreen->setOutput(count->getClickOutput());
    metronomeSoundScreen->setVolume(count->getClickVolume());
    countMetronomeScreen->setRate(count->getRate());
    metronomeSoundScreen->setSound(count->getSound());
    countMetronomeScreen->setInPlay(count->isEnabledInPlay());
    countMetronomeScreen->setInRec(count->isEnabledInRec());
    countMetronomeScreen->setWaitForKey(count->isWaitForKeyEnabled());
    mpcSequencer->setCountEnabled(count->isEnabled());

    auto midiInput = allParser.getMidiInput();

    mpc.screens->get<MidiOutputScreen>("midi-output")->setSoftThru(midiInput->getSoftThruMode());

    auto midiInputScreen = mpc.screens->get<MidiInputScreen>("midi-input");

    midiInputScreen->setReceiveCh(midiInput->getReceiveCh());
    midiInputScreen->setType(midiInput->getFilterType());
    midiInputScreen->setMidiFilterEnabled(midiInput->isFilterEnabled());
    midiInputScreen->setSustainPedalToDuration(midiInput->isSustainPedalToDurationEnabled());

    auto trackDests = midiInput->getMultiRecTrackDests();

    auto multiRecordingSetupScreen = mpc.screens->get<MultiRecordingSetupScreen>("multi-recording-setup");

    for (int i = 0; i < trackDests.size(); i++)
        multiRecordingSetupScreen->getMrsLines()[i]->setTrack(trackDests[i]);

    midiInputScreen->setChPressurePassEnabled(midiInput->isChPressurePassEnabled());
    midiInputScreen->setExclusivePassEnabled(midiInput->isExclusivePassEnabled());
    mpcSequencer->setRecordingModeMulti(midiInput->isMultiRecEnabled());
    midiInputScreen->setNotePassEnabled(midiInput->isNotePassEnabled());
    midiInputScreen->setPgmChangePassEnabled(midiInput->isPgmChangePassEnabled());
    midiInputScreen->setPitchBendPassEnabled(midiInput->isPitchBendPassEnabled());
    midiInputScreen->setPolyPressurePassEnabled(midiInput->isPolyPressurePassEnabled());
    midiInputScreen->getCcPassEnabled() = midiInput->getCcPassEnabled();

    auto midiSyncMisc = allParser.getMidiSync();

    auto misc = allParser.getMisc();
    auto stepEditOptionsScreen = mpc.screens->get<StepEditOptionsScreen>("step-edit-options");
    stepEditOptionsScreen->setAutoStepIncrementEnabled(misc->isAutoStepIncEnabled());
    stepEditOptionsScreen->setDurationOfRecordedNotesTcValue(misc->isDurationOfRecNotesTc());
    stepEditOptionsScreen->setTcValueRecordedNotes(misc->getDurationTcPercentage());

    auto midiSwScreen = mpc.screens->get<MidiSwScreen>("midi-sw");
    midiSwScreen->setSwitches(misc->getSwitches());

    auto othersScreen = mpc.screens->get<OthersScreen>("others");

    othersScreen->setTapAveraging(misc->getTapAvg());

    auto syncScreen = mpc.screens->get<SyncScreen>("sync");

    syncScreen->receiveMMCEnabled = misc->isInReceiveMMCEnabled();
    syncScreen->sendMMCEnabled = midiSyncMisc->isSendMMCEnabled();
    syncScreen->in = midiSyncMisc->getInput();
    syncScreen->out = midiSyncMisc->getOutput();

    if (!mpc.isPluginModeEnabled())
    {
        syncScreen->setModeIn(midiSyncMisc->getInMode());
        syncScreen->setModeOut(midiSyncMisc->getOutMode());
    }

    syncScreen->shiftEarly = midiSyncMisc->getShiftEarly();
    syncScreen->frameRate = midiSyncMisc->getFrameRate();

    mpcSequencer->setSecondSequenceEnabled(allParserSequencer->secondSeqEnabled);

    auto secondSequenceScreen = mpc.screens->get<SecondSeqScreen>("second-seq");
    secondSequenceScreen->sq = allParserSequencer->secondSeqIndex;

    auto songScreen = mpc.screens->get<SongScreen>("song");
    songScreen->setOffset(-1);
    songScreen->setDefaultSongName(midiSyncMisc->getDefSongName());

    auto songs = allParser.getSongs();

    for (int i = 0; i < 20; i++)
    {
        auto allSong = songs[i];
        auto mpcSong = mpcSequencer->getSong(i);
        mpcSong->setUsed(false);

        if (allSong->getIsUsed())
        {
            mpcSong->setUsed(true);
            mpcSong->setName(StrUtil::trim(allSong->name));
            mpcSong->setFirstStep(allSong->getLoopFirstStepIndex());
            mpcSong->setLastStep(allSong->getLoopLastStepIndex());
            mpcSong->setLoopEnabled(allSong->isLoopEnabled());

            auto steps = allSong->getSteps();

            for (int j = 0; j < steps.size(); j++)
            {
                mpcSong->insertStep(mpcSong->getStepCount());
                auto step = mpcSong->getStep(j).lock();
                step->setSequence(steps[j].first);
                step->setRepeats(steps[j].second);
            }
        }
    }
}

std::vector<std::shared_ptr<Sequence>> AllLoader::loadOnlySequencesFromFile(mpc::Mpc& mpc, mpc::disk::MpcFile* f)
{
    std::vector<std::shared_ptr<Sequence>> mpcSequences;

    AllParser allParser(mpc, f->getBytes());
    
    auto allSequences = allParser.getAllSequences();
    
    auto allSeqNames = allParser.getSeqNames()->getNames();
    int counter = 0;

    for (int i = 0; i < 99; i++)
    {
        if (allSeqNames[i].find("(Unused)") != std::string::npos)
        {
            mpcSequences.push_back({});
            continue;
        }
        
        auto mpcSeq = std::make_shared<Sequence>(mpc);
        
        allSequences[counter++]->applyToMpcSeq(mpcSeq);
        
        mpcSequences.push_back(mpcSeq);
    }
    
    return mpcSequences;
}
