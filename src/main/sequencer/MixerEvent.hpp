#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class MixerEvent final : public Event
    {
    public:
        void setParameter(int i) const;
        int getParameter() const;
        void setPadNumber(int i) const;
        int getPad() const;
        void setValue(int i) const;
        int getValue() const;

        explicit MixerEvent(
            const std::function<EventState()>
                &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "mixer";
        }
    };
} // namespace mpc::sequencer
