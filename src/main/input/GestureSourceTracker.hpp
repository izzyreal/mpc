#pragma once

#include "hardware/ComponentId.hpp"

#include <unordered_map>
#include <vector>

namespace mpc::input
{
    class GestureSourceTracker
    {
    private:
        using ComponentId = hardware::ComponentId;
        using SourceId = int;

        struct ComponentGesture
        {
            SourceId sourceId;
            bool mayUpdate;
        };

    public:
        GestureSourceTracker() = default;

        void beginGesture(ComponentId componentId, SourceId sourceId,
                          int maxUpdatingSourceCount)
        {
            if (gestures.count(componentId) == 0)
            {
                constexpr bool mayUpdate = true;
                gestures[componentId] = {{sourceId, mayUpdate}};
            }
            else
            {
                auto &componentGestures = gestures[componentId];
                const bool mayUpdate =
                    componentGestures.size() <
                    static_cast<size_t>(maxUpdatingSourceCount);
                componentGestures.emplace_back(
                    ComponentGesture{sourceId, mayUpdate});
            }
        }

        void endGesture(ComponentId componentId, SourceId sourceId)
        {
            if (gestures.count(componentId) == 0)
            {
                return;
            }

            auto &componentGestures = gestures[componentId];

            for (auto it = componentGestures.begin();
                 it != componentGestures.end(); ++it)
            {
                if (it->sourceId == sourceId)
                {
                    componentGestures.erase(it);
                    break;
                }
            }

            if (componentGestures.empty())
            {
                gestures.erase(componentId);
            }
        }

        bool updateGesture(ComponentId componentId, SourceId sourceId)
        {
            if (gestures.count(componentId) == 0)
            {
                // For 'update' gestures that have no begin and end, i.e. mouse
                // wheel events
                return true;
            }

            for (auto &gesture : gestures[componentId])
            {
                if (gesture.sourceId == sourceId)
                {
                    return gesture.mayUpdate;
                }
            }

            return false;
        }

        int getGestureCount(ComponentId id)
        {
            if (gestures.count(id) == 0)
            {
                return 0;
            }
            return static_cast<int>(gestures[id].size());
        }

    private:
        std::unordered_map<ComponentId, std::vector<ComponentGesture>> gestures;
    };
} // namespace mpc::input
