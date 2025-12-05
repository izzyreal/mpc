#include "sequencer/Step.hpp"

using namespace mpc::sequencer;

void Step::setSequence(const SequenceIndex i)
{
    sequenceIndex = std::clamp(i, MinSequenceIndex, MaxSequenceIndex);
}

mpc::SequenceIndex Step::getSequenceIndex() const
{
    return sequenceIndex;
}

void Step::setRepeats(const int i)
{
    repeatCount = std::clamp(i, 0, 99);
}

int Step::getRepeats() const
{
    return repeatCount;
}
