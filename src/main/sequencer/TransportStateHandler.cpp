#include "sequencer/TransportStateHandler.hpp"

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
                                         const TransportMessage &msg,
                                         const bool autoRefreshPlaybackState)
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;

            if constexpr (std::is_same_v<T, SetPositionQuarterNotes>)
            {
                state.positionQuarterNotes = m.positionQuarterNotes;
            }
            else if constexpr (std::is_same_v<T,
                                              SetPlayStartPositionQuarterNotes>)
            {
                state.playStartPositionQuarterNotes = m.positionQuarterNotes;
            }
            else if constexpr (std::is_same_v<T, BumpPositionByTicks>)
            {
                const double delta = Sequencer::ticksToQuarterNotes(m.ticks);
                state.positionQuarterNotes += delta;
            }
            else if constexpr (std::is_same_v<T, Stop>)
            {
                applyStopMessage(state);
            }
            else if constexpr (std::is_same_v<T, Play>)
            {
                applyPlayMessage(state);
            }
            else if constexpr (std::is_same_v<T, Record>)
            {
                state.recording = true;
                applyMessage(state, Play{});
            }
            else if constexpr (std::is_same_v<T, RecordFromStart>)
            {
                state.recording = true;
                applyMessage(state, PlayFromStart{});
            }
            else if constexpr (std::is_same_v<T, Overdub>)
            {
                state.overdubbing = true;
                applyMessage(state, Play{});
            }
            else if constexpr (std::is_same_v<T, OverdubFromStart>)
            {
                state.overdubbing = true;
                applyMessage(state, PlayFromStart{});
            }
            else if constexpr (std::is_same_v<T, UpdateRecording>)
            {
                state.recording = m.recording;
            }
            else if constexpr (std::is_same_v<T, UpdateOverdubbing>)
            {
                state.overdubbing = m.overdubbing;
            }
            else if constexpr (std::is_same_v<T, SwitchRecordToOverdub>)
            {
                state.recording = false;
                state.overdubbing = true;
                applyMessage(state, Play{});
            }
            else if constexpr (std::is_same_v<T, PlayFromStart>)
            {
                if (state.positionQuarterNotes == 0)
                {
                    applyMessage(state, Play{});
                }
                else
                {
                    state.positionQuarterNotes = 0;
                    auto onComplete = [this]
                    {
                        manager->enqueue(Play{});
                    };
                    manager->enqueue(
                        RefreshPlaybackStateWhileNotPlaying{onComplete});
                }
            }
            else if constexpr (std::is_same_v<T, UpdateCountEnabled>)
            {
                state.countEnabled = m.enabled;
            }
        },
        msg);

    if (!autoRefreshPlaybackState)
    {
        return;
    }

    const auto isPlaying = sequencer->getTransport()->isPlaying();
    const auto playbackState =
        sequencer->getStateManager()->getSnapshot().getPlaybackState();

    if (std::holds_alternative<SetPositionQuarterNotes>(msg))
    {
        if (isPlaying)
        {
            return;
        }

        manager->publishState();

        if (state.positionQuarterNotes >
                playbackState.safeValidUntilQuarterNote ||
            state.positionQuarterNotes < playbackState.safeValidFromQuarterNote)
        {
            manager->applyMessage(RefreshPlaybackStateWhileNotPlaying{});
        }

        return;
    }

    if (isPlaying &&
        isVariantAnyOf(
            msg, TransportMessagesThatInvalidatePlaybackStateWhilePlaying{}))
    {
        manager->publishState();
        manager->applyMessage(RefreshPlaybackStateWhilePlaying{});
    }
    else if (!isPlaying &&
             isVariantAnyOf(
                 msg,
                 TransportMessagesThatInvalidatePlaybackStateWhileNotPlaying{}))
    {
        manager->publishState();
        manager->applyMessage(RefreshPlaybackStateWhileNotPlaying{});
    }
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

        state.playStartPositionQuarterNotes = state.positionQuarterNotes;
        state.sequencerRunning = true;
    }
}

void TransportStateHandler::applyStopMessage(TransportState &state) noexcept
{
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
    state.recording = false;
    state.overdubbing = false;
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
