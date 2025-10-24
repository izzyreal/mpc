#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class Sequence;
}

namespace mpc::sequencer
{
    class TempoChangeEvent
        : public Event
    {

    private:
        int ratio = 1000;
        Sequence *parent = nullptr;

    public:
        void plusOneBar(TempoChangeEvent *next);
        void minusOneBar(TempoChangeEvent *previous);
        void plusOneBeat(TempoChangeEvent *next);
        void minusOneBeat(TempoChangeEvent *previous);
        void plusOneClock(TempoChangeEvent *next);
        void minusOneClock(TempoChangeEvent *previous);
        void setRatio(int i);
        int getRatio() const;
        int getBar(int n, int d);
        int getBeat(int n, int d);
        int getClock(int denominator);
        double getTempo();

    public:
        // Smelly smelly smelly
        void setParent(Sequence *newParent);

    public:
        TempoChangeEvent(Sequence *parent);
        TempoChangeEvent(Sequence *parent, int ratio);
        TempoChangeEvent(Sequence *parent, int ratio, int step);
        TempoChangeEvent(const TempoChangeEvent &);

        std::string getTypeName() override
        {
            return "tempo-change";
        }
    };
} // namespace mpc::sequencer
