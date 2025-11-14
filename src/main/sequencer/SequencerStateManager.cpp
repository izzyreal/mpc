#include "sequencer/SequencerStateManager.hpp"

#include "sequencer/Transport.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"

#include "engine/SequencerPlaybackEngine.hpp"

#include "lcdgui/ScreenId.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui;

SequencerStateManager::SequencerStateManager(Sequencer *sequencer)
    : AtomicStateExchange([](SequencerState &) {}), sequencer(sequencer)
{
}

void SequencerStateManager::applyMessage(const SequencerMessage &msg) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;
            auto &s = activeState;
            auto &t = activeState.transportState;

            if constexpr (std::is_same_v<T, SetPositionQuarterNotes>)
            {
                t.positionQuarterNotes = m.positionQuarterNotes;
                publishState();
            }
            else if constexpr (std::is_same_v<T,
                                              SetPlayStartPositionQuarterNotes>)
            {
                t.playStartPositionQuarterNotes = m.positionQuarterNotes;
                publishState();
            }
            else if constexpr (std::is_same_v<T, BumpPositionByTicks>)
            {
                const double delta = Sequencer::ticksToQuarterNotes(m.ticks);
                t.positionQuarterNotes += delta;
                publishState();
            }
            else if constexpr (std::is_same_v<T, SwitchToNextSequence>)
            {
                constexpr bool setPositionTo0 = false;
                enqueue(
                    SetSelectedSequenceIndex{m.sequenceIndex, setPositionTo0});
                enqueue(Stop{});
                constexpr bool fromStart = true;
                enqueue(Play{fromStart});
            }
            else if constexpr (std::is_same_v<T, SetSelectedSequenceIndex>)
            {
                activeState.selectedSequenceIndex = m.sequenceIndex;

                if (m.setPositionTo0)
                {
                    sequencer->getTransport()->setPosition(0);
                    drainQueue();
                }
            }
            else if constexpr (std::is_same_v<T, Stop>)
            {
                sequencer->getTransport()->stop();
            }
            else if constexpr (std::is_same_v<T, Play>)
            {
                applyPlayMessage(m.fromStart);
            }
        },
        msg);
}

void SequencerStateManager::applyPlayMessage(
    const bool fromStart) const noexcept
{
    const auto transport = sequencer->getTransport();

    if (transport->isPlaying())
    {
        return;
    }

    transport->setEndOfSong(false);

    const auto songScreen =
        sequencer->getScreens()->get<ScreenId::SongScreen>();

    const auto currentSong =
        sequencer->getSong(songScreen->getSelectedSongIndex());

    const bool songMode = sequencer->isSongModeEnabled();

    if (songMode)
    {
        if (!currentSong->isUsed())
        {
            return;
        }

        if (fromStart)
        {
            const int oldSongSequenceIndex = sequencer->getSongSequenceIndex();
            songScreen->setOffset(-1);
            if (sequencer->getSongSequenceIndex() != oldSongSequenceIndex)
            {
                sequencer->setSelectedSequenceIndex(
                    sequencer->getSongSequenceIndex(), true);
            }
        }

        if (songScreen->getOffset() + 1 > currentSong->getStepCount() - 1)
        {
            return;
        }

        int step = songScreen->getOffset() + 1;

        if (step > currentSong->getStepCount())
        {
            step = currentSong->getStepCount() - 1;
        }

        if (const std::shared_ptr<Step> currentStep =
                currentSong->getStep(step).lock();
            !sequencer->getSequence(currentStep->getSequence())->isUsed())
        {
            return;
        }
    }

    const auto countMetronomeScreen =
        sequencer->getScreens()->get<ScreenId::CountMetronomeScreen>();

    const auto countInMode = countMetronomeScreen->getCountInMode();

    std::optional<int64_t> positionQuarterNotesToStartPlayingFrom =
        std::nullopt;

    if (!transport->isCountEnabled() || countInMode == 0 ||
        (countInMode == 1 && !transport->isRecordingOrOverdubbing()))
    {
        if (fromStart && activeState.transportState.positionQuarterNotes != 0)
        {
            positionQuarterNotesToStartPlayingFrom = 0;
        }
    }

    const auto activeSequence = sequencer->getSelectedSequence();

    if (transport->isCountEnabled() && !songMode)
    {
        if (countInMode == 2 ||
            (countInMode == 1 && transport->isRecordingOrOverdubbing()))
        {
            if (fromStart)
            {
                positionQuarterNotesToStartPlayingFrom =
                    Sequencer::ticksToQuarterNotes(
                        activeSequence->getLoopStart());
            }
            else
            {
                positionQuarterNotesToStartPlayingFrom =
                    Sequencer::ticksToQuarterNotes(
                        activeSequence->getFirstTickOfBar(
                            transport->getCurrentBarIndex()));
            }

            transport->setCountInStartPosTicks(Sequencer::quarterNotesToTicks(
                activeState.transportState.positionQuarterNotes));

            transport->setCountInEndPosTicks(activeSequence->getLastTickOfBar(
                transport->getCurrentBarIndex()));
            transport->setCountingIn(true);
        }
    }

    if (positionQuarterNotesToStartPlayingFrom)
    {
        transport->setPosition(*positionQuarterNotesToStartPlayingFrom);
    }

    if (!songMode)
    {
        if (!activeSequence->isUsed())
        {
            transport->setRecording(false);
            transport->setOverdubbing(false);
            return;
        }

        activeSequence->initLoop();

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
        transport->getSequencerPlaybackEngine()->start();
    }
}

void SequencerStateManager::enqueue(SequencerMessage &&m) const noexcept
{
    /*
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;

            if constexpr (std::is_same_v<T, SetPositionQuarterNotes>)
            {
                printf("[Sequencer] SetPositionQuarterNotes: %.3f\n",
                       m.positionQuarterNotes);
            }
            else if constexpr (std::is_same_v<T,
    SetPlayStartPositionQuarterNotes>)
            {
                printf("[Sequencer] SetPlayStartPositionQuarterNotes: %.3f\n",
                       m.positionQuarterNotes);
            }
            else if constexpr (std::is_same_v<T, BumpPositionByTicks>)
            {
                printf("[Sequencer] BumpPositionByTicks: %d ticks (%.6f qn)\n",
                       m.ticks, Sequencer::ticksToQuarterNotes(m.ticks));
            }
        },
        m);
        */

    AtomicStateExchange::enqueue(std::move(m));
}
