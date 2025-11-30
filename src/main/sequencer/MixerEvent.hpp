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
            EventState *eventState,
            const std::function<void(SequenceMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "mixer";
        }
    };
} // namespace mpc::sequencer
