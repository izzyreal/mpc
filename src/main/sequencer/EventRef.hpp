#pragma once
#include "IntTypes.hpp"
#include "TrackMessage.hpp"
#include "sequencer/EventData.hpp"

#include <functional>

namespace mpc::sequencer
{
    class EventRef
    {
    public:
        explicit EventRef(EventData *ptr, const EventData &snapshot,
                          const std::function<void(TrackMessage &&)> &dispatch);
        EventRef(const EventRef &);

        virtual ~EventRef() = default;

        void setTick(int tick) const;
        int getTick() const;
        void setTrack(TrackIndex) const;
        TrackIndex getTrack() const;

        virtual std::string getTypeName() const = 0;

        EventData *const handle = nullptr;
        const EventData snapshot;
        std::function<void(TrackMessage &&)> dispatch;
    };
} // namespace mpc::sequencer
