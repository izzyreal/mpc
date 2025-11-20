#include "performance/EventMapper.hpp"

#include "PerformanceManager.hpp"
#include "sequencer/NoteEvent.hpp"

std::shared_ptr<mpc::sequencer::Event>
mpc::performance::mapPerformanceEventToSequencerEvent(
    std::shared_ptr<performance::PerformanceManager> performanceManager,
    const performance::Event &event)
{
    if (event.type == performance::EventType::NoteOn)
    {
        auto claimedEvent = performanceManager->claimEvent(event);

        auto getSnapshot = [performanceManager, eventIndex = claimedEvent.eventIndex]
        {
            return performanceManager->eventPool.events[eventIndex];
        };

        auto result = std::make_shared<sequencer::NoteOnEvent>(getSnapshot);
        return result;
    }
    return {};
}