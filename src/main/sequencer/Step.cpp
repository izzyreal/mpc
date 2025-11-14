#include "sequencer/Step.hpp"

using namespace mpc::sequencer;

void Step::setSequence(const SequenceIndex i)
{
    sequence = std::clamp(i, MinSequenceIndex, MaxSequenceIndex);
}

mpc::SequenceIndex Step::getSequence() const
{
    return sequence;
}

void Step::setRepeats(const int i)
{
    repeatCount = std::clamp(i, 0, 99);
}

int Step::getRepeats() const
{
    return repeatCount;
}
