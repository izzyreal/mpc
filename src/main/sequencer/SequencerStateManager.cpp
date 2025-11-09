#include "sequencer/SequencerStateManager.hpp"

#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;

using Base = mpc::concurrency::AtomicStateExchange<SequencerState, SequencerStateView, SequencerMessage>;

SequencerStateManager::SequencerStateManager()
    : Base([](SequencerState &){})
{
}

void SequencerStateManager::applyMessage(const SequencerMessage& msg) noexcept
{
    std::visit([&](auto&& m)
    {
        using T = std::decay_t<decltype(m)>;
        auto& s = activeState;

        if constexpr (std::is_same_v<T, SetPositionQuarterNotes>)
        {
            s.positionQuarterNotes = m.positionQuarterNotes;
            publishState();
        }
        else if constexpr (std::is_same_v<T, SetPlayStartPositionQuarterNotes>)
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
    }, msg);
}

