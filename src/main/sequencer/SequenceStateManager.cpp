#include "sequencer/SequenceStateManager.hpp"

using namespace mpc::sequencer;

SequenceStateManager::SequenceStateManager()
    : AtomicStateExchange([](SequenceState &) {})
{
}

void SequenceStateManager::applyMessage(const SequenceMessage &msg) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T, UpdateSequence>)
            {
            }
        },
        msg);
}
