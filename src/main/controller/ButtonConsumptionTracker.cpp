#include "ButtonConsumptionTracker.hpp"

using namespace mpc::controller;

void ButtonConsumptionTracker::addConsumptionRule(
    const hardware::ComponentId watchedId,
    const hardware::ComponentId consumerId)
{
    sessions[watchedId].consumables.push_back(consumerId);
}

void ButtonConsumptionTracker::onPress(const hardware::ComponentId id)
{
    sessions[id].down = true;

    for (auto &pair : sessions)
    {
        auto &session = pair.second;
        if (!session.down)
        {
            continue;
        }
        for (const auto other : session.consumables)
        {
            if (other == id)
            {
                session.consumed = true;
                break;
            }
        }
    }
}

void ButtonConsumptionTracker::onRelease(const hardware::ComponentId id)
{
    if (const auto it = sessions.find(id); it != sessions.end())
    {
        it->second.down = false;
    }
}

bool ButtonConsumptionTracker::isConsumed(
    const hardware::ComponentId watchedId) const
{
    if (const auto it = sessions.find(watchedId); it != sessions.end())
    {
        return it->second.consumed;
    }
    return false;
}

void ButtonConsumptionTracker::reset(const hardware::ComponentId watchedId)
{
    if (const auto it = sessions.find(watchedId); it != sessions.end())
    {
        it->second.consumed = false;
    }
}
