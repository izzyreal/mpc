#include "sequencer/Step.hpp"

using namespace mpc::sequencer;

void Step::setSequence(int i)
{
    if (i < 0 || i > 98)
    {
        return;
    }

    sequence = i;
}

int Step::getSequence()
{
    return sequence;
}

void Step::setRepeats(int i)
{
    if (i < 0 || i > 99)
    {
        return;
    }

    repeatCount = i;
}

int Step::getRepeats()
{
    return repeatCount;
}
