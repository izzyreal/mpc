#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class MixerEvent final : public Event
    {

        int mixerParameter{0};
        int padNumber{0};
        int mixerParameterValue{0};

    public:
        void setParameter(int i);
        int getParameter() const;
        void setPadNumber(int i);
        int getPad() const;
        void setValue(int i);
        int getValue() const;

        explicit MixerEvent(const std::function<performance::Event()> &getSnapshot);
        MixerEvent(const MixerEvent &);

        std::string getTypeName() const override
        {
            return "mixer";
        }
    };
} // namespace mpc::sequencer
