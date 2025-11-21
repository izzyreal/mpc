#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class Sequence;
}

namespace mpc::sequencer
{
    class TempoChangeEvent final : public Event
    {

        int ratio = 1000;
        Sequence *parent = nullptr;

    public:
        void plusOneBar(const TempoChangeEvent *next);
        void minusOneBar(const TempoChangeEvent *previous);
        void plusOneBeat(const TempoChangeEvent *next);
        void minusOneBeat(const TempoChangeEvent *previous);
        void plusOneClock(const TempoChangeEvent *next);
        void minusOneClock(const TempoChangeEvent *previous);
        void setRatio(int i);
        int getRatio() const;
        int getBar(int n, int d) const;
        int getBeat(int n, int d) const;
        int getClock(int denominator) const;
        double getTempo() const;

        void setParent(Sequence *newParent);

        explicit TempoChangeEvent(const std::function<sequencer::EventState()> &getSnapshot, Sequence *parent);
        TempoChangeEvent(const std::function<sequencer::EventState()> &getSnapshot, Sequence *parent, int ratio);
        TempoChangeEvent(const TempoChangeEvent &);

        std::string getTypeName() const override
        {
            return "tempo-change";
        }
    };
} // namespace mpc::sequencer
