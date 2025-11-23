#include "sequencer/SequenceStateManager.hpp"

#include "sequencer/Sequence.hpp"

using namespace mpc::sequencer;

SequenceStateManager::SequenceStateManager(Sequence *sequence)
    : AtomicStateExchange([](SequenceState &) {}), sequence(sequence)
{
}

SequenceStateManager::~SequenceStateManager()
{
    //    printf("~SequenceStateManager\n");
}

void SequenceStateManager::applyMessage(const SequenceMessage &msg) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T, UpdateBarLength>)
            {
                activeState.barLengths[m.barIndex] = m.length;
            }
            else if constexpr (std::is_same_v<T, UpdateBarLengths>)
            {
                activeState.barLengths = m.barLengths;
            }
        },
        msg);
}
