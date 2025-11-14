#pragma once
#include "IntTypes.hpp"

namespace mpc::sequencer
{
    class Step
    {
        SequenceIndex sequence{MinSequenceIndex};
        int repeatCount = 1;

    public:
        void setSequence(SequenceIndex);
        SequenceIndex getSequence() const;
        void setRepeats(int i);
        int getRepeats() const;
    };
} // namespace mpc::sequencer
