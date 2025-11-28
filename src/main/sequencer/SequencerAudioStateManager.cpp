#include "sequencer/SequencerAudioStateManager.hpp"

using namespace mpc::sequencer;

SequencerAudioStateManager::SequencerAudioStateManager(Sequencer *sequencer)
    : AtomicStateExchange([](SequencerAudioState &) {}), sequencer(sequencer)
{
}

SequencerAudioStateManager::~SequencerAudioStateManager()
{
}

void SequencerAudioStateManager::applyMessage(const SequencerAudioMessage &msg) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;

            if constexpr (std::is_same_v<T, SetTimeInSamples>)
            {
                activeState.timeInSamples = m.timeInSamples;
                // printf("Time in samples set to %i\n", activeState.timeInSamples);
            }
        },
        msg);
}

