#include "sequencer/SequencerStateManager.hpp"

#include "Transport.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;

using Base =
    mpc::concurrency::AtomicStateExchange<SequencerState, SequencerStateView,
                                          SequencerMessage>;

SequencerStateManager::SequencerStateManager(Sequencer *sequencer)
    : Base([](SequencerState &) {}), sequencer(sequencer)
{
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
            else if constexpr (std::is_same_v<T, SetSongModeEnabled>)
            {
                printf("[Sequencer] SetSongModeEnabled: %s\n",
                       m.songModeEnabled ? "true" : "false");
            }
        },
        m);
        */

    AtomicStateExchange::enqueue(std::move(m));
}

void SequencerStateManager::applyMessage(const SequencerMessage &msg) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;
            auto &s = activeState;

            if constexpr (std::is_same_v<T, SetPositionQuarterNotes>)
            {
                s.positionQuarterNotes = m.positionQuarterNotes;
                publishState();
            }
            else if constexpr (std::is_same_v<T,
                                              SetPlayStartPositionQuarterNotes>)
            {
                s.playStartPositionQuarterNotes = m.positionQuarterNotes;
                publishState();
            }
            else if constexpr (std::is_same_v<T, BumpPositionByTicks>)
            {
                const double delta = Sequencer::ticksToQuarterNotes(m.ticks);
                s.positionQuarterNotes += delta;
                publishState();
            }
            else if constexpr (std::is_same_v<T, SetSongModeEnabled>)
            {
                s.songModeEnabled = m.songModeEnabled;
            }
            else if constexpr (std::is_same_v<T, SwitchToNextSequence>)
            {
                constexpr bool setPositionTo0 = false;
                enqueue(SetActiveSequenceIndex{m.sequenceIndex, setPositionTo0});
                enqueue(Stop{});
                enqueue(PlayFromStart{});
            }
            else if constexpr (std::is_same_v<T, SetActiveSequenceIndex>)
            {
                sequencer->setActiveSequenceIndex(m.sequenceIndex, false);

                if (m.setPositionTo0)
                {
                    enqueue(SetPositionQuarterNotes{0.0});
                }
            }
            else if constexpr (std::is_same_v<T, Stop>)
            {
                sequencer->getTransport()->stop();
            }
            else if constexpr (std::is_same_v<T, PlayFromStart>)
            {
                sequencer->getTransport()->playFromStart();
            }
        },
        msg);
}
