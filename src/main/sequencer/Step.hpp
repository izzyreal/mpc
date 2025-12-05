#pragma once
#include "IntTypes.hpp"

namespace mpc::sequencer
{
    class Step
    {
        SequenceIndex sequenceIndex{MinSequenceIndex};
        int repeatCount = 1;

    public:
        void setSequence(SequenceIndex);
        SequenceIndex getSequenceIndex() const;
        void setRepeats(int i);
        int getRepeats() const;
    };
} // namespace mpc::sequencer
