#include "sequencer/TrackEventStateManager.hpp"

using namespace mpc::sequencer;

TrackEventStateManager::TrackEventStateManager()
    : AtomicStateExchange([](TrackEventState &) {})
{
}

void TrackEventStateManager::applyMessage(const TrackEventMessage &msg) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T, UpdateTrackEvent>)
            {
            }
        },
        msg);
}
