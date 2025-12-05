#pragma once

#include <unordered_map>
#include <vector>

#include "hardware/ComponentId.hpp"

namespace mpc::controller
{
    class ButtonConsumptionTracker
    {
    public:
        void onPress(hardware::ComponentId id);
        void onRelease(hardware::ComponentId id);
        void addConsumptionRule(hardware::ComponentId watchedId,
                                hardware::ComponentId consumerId);
        bool isConsumed(hardware::ComponentId watchedId) const;
        void reset(hardware::ComponentId watchedId);

    private:
        struct Session
        {
            bool down = false;
            bool consumed = false;
            std::vector<hardware::ComponentId> consumables;
        };

        std::unordered_map<hardware::ComponentId, Session> sessions;
    };
}
