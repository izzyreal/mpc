#include "sequencer/TransportStateHandler.hpp"

#include "Clock.hpp"
#include "Sequence.hpp"
#include "Sequencer.hpp"
#include "SequencerStateManager.hpp"
#include "lcdgui/ScreenId.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"
#include "sequencer/TransportState.hpp"
#include "utils/VariantUtils.hpp"

using namespace mpc::sequencer;

TransportStateHandler::TransportStateHandler(SequencerStateManager *manager,
                                             Sequencer *sequencer)
    : manager(manager), sequencer(sequencer)
{
}

TransportStateHandler::~TransportStateHandler() {}

void TransportStateHandler::installCountIn(TransportState &state,
                                           const bool fromStart) const
{
    if (!state.countEnabled)
    {
        return;
    }

    const auto isRecordingOrOverdubbing =
        state.overdubbingEnabled || state.recordingEnabled;

    const auto countMetronomeScreen =
        sequencer->getScreens()->get<lcdgui::ScreenId::CountMetronomeScreen>();

    const auto countInMode = countMetronomeScreen->getCountInMode();

    if (countInMode == 0 || (countInMode == 1 && !isRecordingOrOverdubbing))
    {
        return;
    }

    const auto activeSequence = sequencer->getSelectedSequence();

    if (fromStart)
    {
        state.countInStartPos = state.positionQuarterNotes;
        state.countInEndPos = activeSequence->getLastTickOfBar(0);
        state.countingIn = true;
        return;
    }

    const auto currentBarIndex =
        activeSequence->getBarIndexForPositionQN(state.positionQuarterNotes);

    const Tick posToStartPlayingFrom =
        activeSequence->getFirstTickOfBar(currentBarIndex);

    state.positionQuarterNotes =
        Sequencer::ticksToQuarterNotes(posToStartPlayingFrom);
    state.countInStartPos = posToStartPlayingFrom;
    state.countInEndPos = activeSequence->getLastTickOfBar(currentBarIndex);
    state.countingIn = true;
}

void TransportStateHandler::applyMessage(TransportState &state,
                                         const TransportMessage &msg)
{
    const auto visitor = Overload{
        [&](const SetPositionQuarterNotes &m)
        {
            state.positionQuarterNotes = m.positionQuarterNotes;
        },
        [&](const StopSequence &)
        {
            applyStopSequence(state);
        },
        [&](const PlaySequence &)
        {
            installCountIn(state, false);
            applyPlaySequence(state);
        },
        [&](const Record &)
        {
            state.recordingEnabled = true;
            applyMessage(state, PlaySequence{});
        },
        [&](const RecordFromStart &)
        {
            state.recordingEnabled = true;
            applyMessage(state, PlaySequenceFromStart{});
        },
        [&](const Overdub &)
        {
            state.overdubbingEnabled = true;
            applyMessage(state, PlaySequence{});
        },
        [&](const OverdubFromStart &)
        {
            state.overdubbingEnabled = true;
            applyMessage(state, PlaySequenceFromStart{});
        },
        [&](const SetRecordingEnabled &m)
        {
            state.recordingEnabled = m.recording;
        },
        [&](const SetOverdubbingEnabled &m)
        {
            state.overdubbingEnabled = m.overdubbing;
        },
        [&](const SwitchRecordToOverdub &)
        {
            state.recordingEnabled = false;
            state.overdubbingEnabled = true;
        },
        [&](const PlaySequenceFromStart &)
        {
            state.positionQuarterNotes = 0;
            installCountIn(state, true);
            applyPlaySequence(state);
        },
        [&](const SetCountEnabled &m)
        {
            state.countEnabled = m.enabled;
        },
        [&](const SetMetronomeOnlyEnabled &m)
        {
            state.metronomeOnlyEnabled = m.enabled;
        },
        [&](const SetMetronomeOnlyTickPosition &m)
        {
            state.metronomeOnlyPositionTicks = m.position;
        },
        [&](const BumpMetronomeOnlyTickPositionOneTick &)
        {
            state.metronomeOnlyPositionTicks += 1;
        },
        [&](const PlayMetronomeOnly &)
        {
            state.metronomeOnlyPositionTicks = 0;
            state.metronomeOnlyEnabled = true;
            state.sequencerRunning = true;
        },
        [&](const StopMetronomeOnly &)
        {
            state.sequencerRunning = false;
            state.metronomeOnlyEnabled = false;
            state.metronomeOnlyPositionTicks = 0;
        },
        [&](const PlaySong &)
        {
            installCountIn(state, false);
            applyPlaySong(state);
        },
        [&](const PlaySongFromStart &)
        {
            state.positionQuarterNotes = 0;
            const auto songScreen =
                sequencer->getScreens()->get<lcdgui::ScreenId::SongScreen>();

            const int oldSongSequenceIndex = sequencer->getSongSequenceIndex();

            songScreen->setOffset(-1);

            if (sequencer->getSongSequenceIndex() != oldSongSequenceIndex)
            {
                sequencer->setSelectedSequenceIndex(
                    sequencer->getSongSequenceIndex(), true);
                manager->drainQueue();
            }

            installCountIn(state, true);
            applyPlaySong(state);
        },
        [&](const StopSong &)
        {
            applyStopSequence(state);
            applyStopSong(state);
        },
        [&](const SetCountInPositions &m)
        {
            state.countInStartPos = m.start;
            state.countInEndPos = m.end;
        },
        [&](const SetEndOfSongEnabled &m)
        {
            state.endOfSong = m.endOfSongEnabled;
        },
        [&](const SetPlayedSongStepRepetitionCount &m)
        {
            state.playedSongStepRepetitionCount = m.count;
        },
        [&](const SetMasterTempo &m)
        {
            state.tempo = m.tempo;
        },
        [&](const SetTempoSourceIsSequence &m)
        {
            state.tempoSourceIsSequenceEnabled = m.enabled;
        },
        [&](const SetShouldWaitForMidiClockLock &m)
        {
            state.shouldWaitForMidiClockLock = m.enabled;
        },
        [&](const SetCountingIn &m)
        {
            state.countingIn = m.countingIn;
        }};

    std::visit(visitor, msg);
}

void TransportStateHandler::applyPlaySequence(
    TransportState &state) const noexcept
{
    if (state.sequencerRunning)
    {
        return;
    }

    const TransportStateView transport(state);

    if (transport.isRecordingOrOverdubbing())
    {
        sequencer->storeSelectedSequenceInUndoPlaceHolder();
    }

    sequencer->clock->reset();

    if (sequencer->isBouncePrepared())
    {
        sequencer->startBouncing();
    }

    state.playStartPositionQuarterNotes = state.positionQuarterNotes;
    manager->applyMessage(SyncTrackEventIndices{});

    if (sequencer->getScreens()->get<lcdgui::ScreenId::SyncScreen>()->modeOut != 0)
    {
        state.shouldWaitForMidiClockLock = true;
    }

    state.sequencerRunning = true;
}

void TransportStateHandler::applyPlaySong(TransportState &state) const noexcept
{
    state.endOfSong = false;

    const auto countMetronomeScreen =
        sequencer->getScreens()->get<lcdgui::ScreenId::CountMetronomeScreen>();

    const auto activeSequence = sequencer->getSelectedSequence();

    sequencer->clock->reset();

    if (sequencer->isBouncePrepared())
    {
        sequencer->startBouncing();
    }

    state.playStartPositionQuarterNotes = state.positionQuarterNotes;
    manager->applyMessage(SyncTrackEventIndices{});
    state.sequencerRunning = true;
}

void TransportStateHandler::applyStopSequence(
    TransportState &state) const noexcept
{
    state.sequencerRunning = false;

    sequencer->setNextSq(NoSequenceIndex);

    state.recordingEnabled = false;
    state.overdubbingEnabled = false;

    if (state.countingIn)
    {
        state.positionQuarterNotes =
            Sequencer::ticksToQuarterNotes(state.countInStartPos);
        state.countInStartPos = NoTick;
        state.countInEndPos = NoTick;
        state.countingIn = false;
    }

    const auto vmpcDirectToDiskRecorderScreen =
        sequencer->getScreens()
            ->get<lcdgui::ScreenId::VmpcDirectToDiskRecorderScreen>();

    if (sequencer->isBouncing() &&
        vmpcDirectToDiskRecorderScreen->getRecord() != 4)
    {
        sequencer->stopBouncing();
    }
}

void TransportStateHandler::applyStopSong(TransportState &state) const noexcept
{
    state.playedSongStepRepetitionCount = 0;

    if (state.endOfSong)
    {
        const auto songScreen =
            sequencer->getScreens()->get<lcdgui::ScreenId::SongScreen>();

        songScreen->setOffset(songScreen->getOffset() + 1);
    }
}
