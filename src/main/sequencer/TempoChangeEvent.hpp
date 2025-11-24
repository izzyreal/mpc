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
        Sequence *parent = nullptr;

    public:
        void plusOneBar(const TempoChangeEvent *next) const;
        void minusOneBar(const TempoChangeEvent *previous) const;
        void plusOneBeat(const TempoChangeEvent *next) const;
        void minusOneBeat(const TempoChangeEvent *previous) const;
        void plusOneClock(const TempoChangeEvent *next) const;
        void minusOneClock(const TempoChangeEvent *previous) const;
        void setRatio(int i) const;
        int getRatio() const;
        int getBar(int n, int d) const;
        int getBeat(int n, int d) const;
        int getClock(int denominator) const;
        double getTempo() const;

        void setParent(Sequence *newParent);

        TempoChangeEvent(
            const std::function<EventState()> &getSnapshot,
            const std::function<void(NonRtSequencerMessage &&)> &dispatch,
            Sequence *parent);

        std::string getTypeName() const override
        {
            return "tempo-change";
        }
    };
} // namespace mpc::sequencer
