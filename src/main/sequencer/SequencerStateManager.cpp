#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::sequencer;

SequencerStateManager::SequencerStateManager(Sequencer *sequencer)
    : AtomicStateExchange([](SequencerState &) {}), sequencer(sequencer)
{
}

SequencerStateManager::~SequencerStateManager()
{
    // printf("~SequencerStateManager\n");
}

void SequencerStateManager::applyMessage(const SequencerMessage &msg) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;

            if constexpr (std::is_same_v<T, SetTimeInSamples>)
            {
                activeState.timeInSamples = m.timeInSamples;
            }
        },
        msg);
}

