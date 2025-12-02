#pragma once
#include <sequencer/EventRef.hpp>

namespace mpc::sequencer
{
    class Sequence;
}

namespace mpc::sequencer
{
    class TempoChangeEvent final : public EventRef
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

        TempoChangeEvent(EventData *ptr, const EventData &snapshot,
                         const std::function<void(TrackMessage &&)> &dispatch,
                         Sequence *parent);

        std::string getTypeName() const override
        {
            return "tempo-change";
        }
    };
} // namespace mpc::sequencer
