#include "sequencer/TransportStateHandler.hpp"

#include "Clock.hpp"
#include "Sequence.hpp"
#include "Sequencer.hpp"
#include "SequencerStateManager.hpp"
#include "Transport.hpp"
#include "lcdgui/ScreenId.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "sequencer/TransportState.hpp"
#include "utils/VariantUtils.hpp"

using namespace mpc::sequencer;

TransportStateHandler::TransportStateHandler(SequencerStateManager *manager,
                                             Sequencer *sequencer)
    : manager(manager), sequencer(sequencer)
{
}

TransportStateHandler::~TransportStateHandler() {}

void TransportStateHandler::applyMessage(TransportState &state,
                                         const TransportMessage &msg)
{
    const auto visitor =
        Overload{[&](const SetPositionQuarterNotes &m)
                 {
                     state.positionQuarterNotes = m.positionQuarterNotes;
                 },
                 [&](const Stop &)
                 {
                     applyStopMessage(state);
                 },
                 [&](const Play &)
                 {
                     applyPlayMessage(state);
                 },
                 [&](const Record &)
                 {
                     state.recordingEnabled = true;
                     applyMessage(state, Play{});
                 },
                 [&](const RecordFromStart &)
                 {
                     state.recordingEnabled = true;
                     applyMessage(state, PlayFromStart{});
                 },
                 [&](const Overdub &)
                 {
                     state.overdubbingEnabled = true;
                     applyMessage(state, Play{});
                 },
                 [&](const OverdubFromStart &)
                 {
                     state.overdubbingEnabled = true;
                     applyMessage(state, PlayFromStart{});
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
                     applyMessage(state, Play{});
                 },
                 [&](const PlayFromStart &)
                 {
                     state.positionQuarterNotes = 0;
                     manager->enqueue(Play{});
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
                 [&](const SetCountingIn &m)
                 {
                     state.countingIn = m.countingIn;
                 }};

    std::visit(visitor, msg);
}

void TransportStateHandler::applyPlayMessage(
    TransportState &state) const noexcept
{
    if (state.sequencerRunning)
    {
        return;
    }

    const auto transport = sequencer->getTransport();

    transport->setEndOfSong(false);

    const auto songScreen =
        sequencer->getScreens()->get<lcdgui::ScreenId::SongScreen>();

    const auto currentSong =
        sequencer->getSong(songScreen->getSelectedSongIndex());

    const bool songMode = sequencer->isSongModeEnabled();

    // if (songMode)
    // {
    //     if (!currentSong->isUsed())
    //     {
    //         return;
    //     }
    //
    //     if (fromStart)
    //     {
    //         const int oldSongSequenceIndex =
    //         sequencer->getSongSequenceIndex(); songScreen->setOffset(-1); if
    //         (sequencer->getSongSequenceIndex() != oldSongSequenceIndex)
    //         {
    //             sequencer->setSelectedSequenceIndex(
    //                 sequencer->getSongSequenceIndex(), true);
    //         }
    //     }
    //
    //     if (songScreen->getOffset() + 1 > currentSong->getStepCount() - 1)
    //     {
    //         return;
    //     }
    //
    //     int step = songScreen->getOffset() + 1;
    //
    //     if (step > currentSong->getStepCount())
    //     {
    //         step = currentSong->getStepCount() - 1;
    //     }
    //
    //     if (const std::shared_ptr<Step> currentStep =
    //             currentSong->getStep(step).lock();
    //         !sequencer->getSequence(currentStep->getSequence())->isUsed())
    //     {
    //         return;
    //     }
    // }

    const auto countMetronomeScreen =
        sequencer->getScreens()->get<lcdgui::ScreenId::CountMetronomeScreen>();

    const auto countInMode = countMetronomeScreen->getCountInMode();

    // std::optional<int64_t> positionQuarterNotesToStartPlayingFrom =
    //     std::nullopt;

    // if (!transport->isCountEnabled() || countInMode == 0 ||
    //     (countInMode == 1 && !transport->isRecordingOrOverdubbing()))
    // {
    //     if (fromStart && activeState.transportState.positionQuarterNotes !=
    //     0)
    //     {
    //         positionQuarterNotesToStartPlayingFrom = 0;
    //     }
    // }
    //
    const auto activeSequence = sequencer->getSelectedSequence();

    // if (transport->isCountEnabled() && !songMode)
    // {
    //     if (countInMode == 2 ||
    //         (countInMode == 1 && transport->isRecordingOrOverdubbing()))
    //     {
    //         if (fromStart)
    //         {
    //             positionQuarterNotesToStartPlayingFrom =
    //                 Sequencer::ticksToQuarterNotes(
    //                     activeSequence->getLoopStart());
    //         }
    //         else
    //         {
    //             positionQuarterNotesToStartPlayingFrom =
    //                 Sequencer::ticksToQuarterNotes(
    //                     activeSequence->getFirstTickOfBar(
    //                         transport->getCurrentBarIndex()));
    //         }
    //
    //         transport->setCountInStartPosTicks(Sequencer::quarterNotesToTicks(
    //             activeState.transportState.positionQuarterNotes));
    //
    //         transport->setCountInEndPosTicks(activeSequence->getLastTickOfBar(
    //             transport->getCurrentBarIndex()));
    //         transport->setCountingIn(true);
    //     }
    // }

    if (!songMode)
    {
        if (!activeSequence->isUsed())
        {
            transport->setRecording(false);
            transport->setOverdubbing(false);
            return;
        }

        if (transport->isRecordingOrOverdubbing())
        {
            sequencer->storeSelectedSequenceInUndoPlaceHolder();
        }
    }

    if (sequencer->isBouncePrepared())
    {
        sequencer->startBouncing();
    }
    else
    {
        // if (positionQuarterNotesToStartPlayingFrom)
        // {
        //     if (*positionQuarterNotesToStartPlayingFrom !=
        //     activeState.transportState.positionQuarterNotes)
        //     {
        //         activeState.transportState.positionQuarterNotes =
        //         *positionQuarterNotesToStartPlayingFrom;
        //
        //     }
        // }

        sequencer->clock->reset();
        state.playStartPositionQuarterNotes = state.positionQuarterNotes;
        manager->applyMessage(SyncTrackEventIndices{});
        state.sequencerRunning = true;
    }
}

void TransportStateHandler::applyStopMessage(TransportState &state) noexcept
{
    state.positionQuarterNotes = Sequencer::ticksToQuarterNotes(
        sequencer->getTransport()->getTickPosition());
    state.sequencerRunning = false;
    // const bool bouncing = sequencer.isBouncing();
    //
    // if (!isPlaying() && !bouncing)
    // {
    //     if (const auto snapshot =
    //     sequencer.getStateManager()->getSnapshot();
    //         snapshot.getPositionQuarterNotes() != 0.0)
    //     {
    //         setPosition(0); // real 2kxl doesn't do this
    //     }
    //
    //     return;
    // }
    //
    // playedStepRepetitions = 0;
    // sequencer.setNextSq(NoSequenceIndex);
    //
    // const auto activeSequence = sequencer.getSelectedSequence();
    // const auto pos = getTickPosition();
    //
    // int64_t newTickPosition = pos;
    //
    // if (pos > activeSequence->getLastTick())
    // {
    //     newTickPosition = activeSequence->getLastTick();
    // }
    //
    state.recordingEnabled = false;
    state.overdubbingEnabled = false;
    //
    // if (countingIn)
    // {
    //     newTickPosition = countInStartPos;
    //     resetCountInPositions();
    //     countingIn = false;
    // }
    //
    // setPosition(Sequencer::ticksToQuarterNotes(newTickPosition));
    //
    // const auto songScreen =
    // sequencer.getScreens()->get<ScreenId::SongScreen>();
    //
    // if (endOfSong)
    // {
    //     songScreen->setOffset(songScreen->getOffset() + 1);
    // }
    //
    // const auto vmpcDirectToDiskRecorderScreen =
    //     sequencer.getScreens()->get<ScreenId::VmpcDirectToDiskRecorderScreen>();
    //
    // if (bouncing && vmpcDirectToDiskRecorderScreen->getRecord() != 4)
    // {
    //     sequencer.stopBouncing();
    // }
    //
}
