#pragma once
#include <sequencer/EventRef.hpp>

namespace mpc::sequencer
{
    class MixerEvent final : public EventRef
    {
    public:
        void setParameter(int i) const;
        int getParameter() const;
        void setPadNumber(int i) const;
        int getPad() const;
        void setValue(int i) const;
        int getValue() const;

        explicit MixerEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "mixer";
        }
    };
} // namespace mpc::sequencer
