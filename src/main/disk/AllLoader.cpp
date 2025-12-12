#include "AllLoader.hpp"

#include "Mpc.hpp"

#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "controller/MidiFootswitchFunctionMap.hpp"
#include "file/all/AllParser.hpp"
#include "file/all/Count.hpp"
#include "file/all/Defaults.hpp"
#include "file/all/MidiInput.hpp"
#include "file/all/MidiSyncMisc.hpp"
#include "file/all/Misc.hpp"
#include "file/all/AllSequence.hpp"
#include "file/all/SequenceNames.hpp"
#include "file/all/AllSequencer.hpp"
#include "file/all/AllSong.hpp"

#include "disk/MpcFile.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Transport.hpp"
#include "sequencer/Song.hpp"

#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/window/MidiOutputScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimeDisplayScreen.hpp"
#include "lcdgui/screens/window/IgnoreTempoChangeScreen.hpp"
#include "lcdgui/screens/window/LocateScreen.hpp"
#include "lcdgui/screens/SecondSeqScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/OthersScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"

#include "lcdgui/screens/dialog/MetronomeSoundScreen.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::disk;
using namespace mpc::file::all;
using namespace mpc::sequencer;

void AllLoader::loadEverythingFromFile(Mpc &mpc, MpcFile *f)
{
    AllParser allParser(mpc, f->getBytes());
    loadEverythingFromAllParser(mpc, allParser);
}

void AllLoader::loadEverythingFromAllParser(Mpc &mpc, AllParser &allParser)
{
    auto mpcSequencer = mpc.getSequencer();
    auto allSequences = allParser.getAllSequences();
    auto allSeqNames = allParser.getSeqNames()->getNames();
    auto defaults = allParser.getDefaults();

    auto userScreen = mpc.screens->get<ScreenId::UserScreen>();

    userScreen->setLastBar(defaults->getBarCount() - 1);
    userScreen->setLoop(defaults->isLoopEnabled());
    userScreen->setBus(busIndexToBusType(defaults->getBusses()[0]));

    mpcSequencer->setDefaultSequenceName(defaults->getDefaultSeqName());
    auto defTrackNames = defaults->getDefaultTrackNames();

    for (int i = 0; i < Mpc2000XlSpecs::TRACK_COUNT; ++i)
    {
        mpcSequencer->setDefaultTrackName(i, defTrackNames[i]);
    }

    userScreen->setDeviceNumber(defaults->getDevices()[0]);
    userScreen->setTimeSig(defaults->getTimeSigNum(),
                           defaults->getTimeSigDen());
    userScreen->setPgm(defaults->getPgms()[0]);
    userScreen->setTempo(defaults->getTempo() * 0.1);
    userScreen->setVelo(defaults->getTrVelos()[0]);

    int index = 0;

    mpcSequencer->deleteAllSequences();

    for (auto &as : allSequences)
    {
        if (as == nullptr)
        {
            index++;
            continue;
        }

        auto mpcSeq = mpcSequencer->getSequence(index++);
        as->applyToInMemorySequence(mpcSeq, mpcSequencer->getStateManager().get());
    }

    auto allParserSequencer = allParser.getSequencer();
    mpcSequencer->setSelectedSequenceIndex(
        SequenceIndex(allParserSequencer->sequence), false);
    mpcSequencer->getTransport()->setPosition(0);
    mpcSequencer->setSelectedTrackIndex(allParserSequencer->track);
    mpcSequencer->getTransport()->setMasterTempo(
        allParserSequencer->masterTempo);
    mpcSequencer->getTransport()->setTempoSourceIsSequence(
        allParserSequencer->tempoSourceIsSequence);

    auto timingCorrectScreen =
        mpc.screens->get<ScreenId::TimingCorrectScreen>();
    timingCorrectScreen->setNoteValue(allParserSequencer->tc);

    auto timeDisplayScreen = mpc.screens->get<ScreenId::TimeDisplayScreen>();
    timeDisplayScreen->setDisplayStyle(allParserSequencer->timeDisplayStyle);

    auto count = allParser.getCount();

    auto countMetronomeScreen =
        mpc.screens->get<ScreenId::CountMetronomeScreen>();
    auto metronomeSoundScreen =
        mpc.screens->get<ScreenId::MetronomeSoundScreen>();

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
    mpcSequencer->getTransport()->setCountEnabled(count->isEnabled());

    auto midiInput = allParser.getMidiInput();

    mpc.screens->get<ScreenId::MidiOutputScreen>()->setSoftThru(
        midiInput->getSoftThruMode());

    auto midiInputScreen = mpc.screens->get<ScreenId::MidiInputScreen>();

    midiInputScreen->setReceiveCh(midiInput->getReceiveCh() - 1);
    midiInputScreen->setType(midiInput->getFilterType());
    midiInputScreen->setMidiFilterEnabled(midiInput->isFilterEnabled());
    midiInputScreen->setSustainPedalToDuration(
        midiInput->isSustainPedalToDurationEnabled());

    auto trackDests = midiInput->getMultiRecTrackDests();

    auto multiRecordingSetupScreen =
        mpc.screens->get<ScreenId::MultiRecordingSetupScreen>();

    for (int i = 0; i < trackDests.size(); i++)
    {
        multiRecordingSetupScreen->getMrsLines()[i]->setTrack(trackDests[i]);
    }

    midiInputScreen->setChPressurePassEnabled(
        midiInput->isChPressurePassEnabled());
    midiInputScreen->setExclusivePassEnabled(
        midiInput->isExclusivePassEnabled());
    mpcSequencer->setRecordingModeMulti(midiInput->isMultiRecEnabled());
    midiInputScreen->setNotePassEnabled(midiInput->isNotePassEnabled());
    midiInputScreen->setPgmChangePassEnabled(
        midiInput->isPgmChangePassEnabled());
    midiInputScreen->setPitchBendPassEnabled(
        midiInput->isPitchBendPassEnabled());
    midiInputScreen->setPolyPressurePassEnabled(
        midiInput->isPolyPressurePassEnabled());
    midiInputScreen->getCcPassEnabled() = midiInput->getCcPassEnabled();

    auto midiSyncMisc = allParser.getMidiSync();

    auto misc = allParser.getMisc();
    auto stepEditOptionsScreen =
        mpc.screens->get<ScreenId::StepEditOptionsScreen>();
    stepEditOptionsScreen->setAutoStepIncrementEnabled(
        misc->isAutoStepIncEnabled());
    stepEditOptionsScreen->setDurationOfRecordedNotesTcValue(
        misc->isDurationOfRecNotesTc());
    stepEditOptionsScreen->setTcValueRecordedNotes(
        misc->getDurationTcPercentage());
    midiInputScreen->setProgChangeSeq(misc->isPgmChToSeqEnabled());

    auto locateScreen = mpc.screens->get<ScreenId::LocateScreen>();
    locateScreen->setLocations(misc->getLocations());

    auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();
    using Cmd = midi::input::MidiControlTarget::SequencerTarget::Command;
    using MT = midi::input::MidiBindingBase::MessageType;

    int bindingCounter = 0;
    for (auto [cc, functionIndex] : misc->getSwitches())
    {
        auto fn =
            static_cast<controller::MidiFootswitchFunction>(functionIndex);

        if (auto hw = controller::footswitchToComponentId.find(fn);
            hw != controller::footswitchToComponentId.end())
        {
            footswitchController->bindings[bindingCounter++] =
                midi::input::HardwareBinding{{-1, cc, std::nullopt,
                                              midi::input::Interaction::Press,
                                              MT::CC},
                                             {hw->second}};
        }
        else if (auto sq = controller::footswitchToSequencerCmd.find(fn);
                 sq != controller::footswitchToSequencerCmd.end())
        {
            footswitchController->bindings[bindingCounter++] =
                midi::input::SequencerBinding{{-1, cc, std::nullopt,
                                               midi::input::Interaction::Press,
                                               MT::CC},
                                              {sq->second}};
        }
        else
        {
            std::cout << "[Footswitch] Unmapped function index: "
                      << functionIndex << "\n";
        }
    }

    auto othersScreen = mpc.screens->get<ScreenId::OthersScreen>();

    othersScreen->setTapAveraging(misc->getTapAvg() + 2);

    auto syncScreen = mpc.screens->get<ScreenId::SyncScreen>();

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

    mpcSequencer->setSecondSequenceEnabled(
        allParserSequencer->secondSeqEnabled);

    auto secondSequenceScreen = mpc.screens->get<ScreenId::SecondSeqScreen>();
    secondSequenceScreen->sq = allParserSequencer->secondSeqIndex;

    auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    songScreen->setDefaultSongName(midiSyncMisc->getDefSongName());
    auto ignoreTempoChangeScreen =
        mpc.screens->get<ScreenId::IgnoreTempoChangeScreen>();
    ignoreTempoChangeScreen->setIgnore(
        midiSyncMisc->getSongModeIgnoreTempoChangeEvents());

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

            auto steps = allSong->getSteps();

            for (int stepIndex = 0; stepIndex < steps.size(); ++stepIndex)
            {
                const auto idx = SongStepIndex(stepIndex);
                mpcSong->insertStep(idx);
                mpcSong->setStepSequenceIndex(
                    idx, SequenceIndex(steps[stepIndex].first));
                mpcSong->setStepRepetitionCount(idx, steps[stepIndex].second);
            }

            mpcSong->setFirstLoopStepIndex(
                SongStepIndex(allSong->getLoopFirstStepIndex()));

            mpcSong->setLastLoopStepIndex(
                SongStepIndex(allSong->getLoopLastStepIndex()));

            mpcSong->setLoopEnabled(allSong->isLoopEnabled());
        }
    }

    mpcSequencer->getStateManager()->enqueue(
        SetSelectedSongStepIndex{SongStepIndex(0), NoSequenceIndex});
}

std::vector<SequenceMetaInfo>
AllLoader::loadSequenceMetaInfosFromFile(Mpc &mpc, MpcFile *f)
{
    std::vector<SequenceMetaInfo> result;
    const AllParser allParser(mpc, f->getBytes());

    const auto allSeqNames = allParser.getSeqNames()->getNames();
    const auto allSeqUsednesses = allParser.getSeqNames()->getUsednesses();

    for (int i = 0; i < Mpc2000XlSpecs::SEQUENCE_COUNT; ++i)
    {
        result.push_back({allSeqUsednesses[i], allSeqNames[i]});
    }

    return result;
}

std::shared_ptr<Sequence>
AllLoader::loadOneSequenceFromFile(Mpc &mpc, MpcFile *f,
                                   const SequenceIndex sourceIndexInAllFile,
                                   const SequenceIndex destIndexInMpcMemory)
{
    AllParser allParser(mpc, f->getBytes());

    const auto allSequences = allParser.getAllSequences();

    const auto allSeqNames = allParser.getSeqNames()->getNames();

    const auto sequencer = mpc.getSequencer();

    const auto sequence = sequencer->getSequence(destIndexInMpcMemory);

    allSequences[sourceIndexInAllFile]->applyToInMemorySequence(
        sequence, sequencer->getStateManager().get());

    return sequence;
}
