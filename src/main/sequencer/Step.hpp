#pragma once

namespace mpc::sequencer
{
    class Step
    {

        int sequence = 0;
        int repeatCount = 1;

    public:
        void setSequence(int i);
        int getSequence() const;
        void setRepeats(int i);
        int getRepeats() const;
    };
} // namespace mpc::sequencer
