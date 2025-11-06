#include "sequencer/Song.hpp"

#include "sequencer/Step.hpp"

#include <algorithm>

using namespace mpc::sequencer;

void Song::setLoopEnabled(bool b)
{
    loopEnabled = b;
}

bool Song::isLoopEnabled() const
{
    return loopEnabled;
}

void Song::setFirstStep(int i)
{
    const auto candidate = std::clamp(i, 0, std::max(0, getStepCount() - 1));

    if (firstStep == candidate)
    {
        return;
    }

    firstStep = candidate;

    if (firstStep > lastStep)
    {
        setLastStep(firstStep);
    }
}

int Song::getFirstStep() const
{
    return firstStep;
}

void Song::setLastStep(int i)
{
    const auto candidate = std::clamp(i, 0, std::max(0, getStepCount() - 1));

    if (lastStep == candidate)
    {
        return;
    }

    lastStep = candidate;

    if (lastStep < firstStep)
    {
        setFirstStep(lastStep);
    }
}

int Song::getLastStep() const
{
    return lastStep;
}

void Song::setName(const std::string &str)
{
    name = str;
}

std::string Song::getName()
{
    if (!used)
    {
        return "(Unused)";
    }

    return name;
}

void Song::deleteStep(int stepIndex)
{
    if (stepIndex >= (int)steps.size())
    {
        return;
    }

    steps.erase(steps.begin() + stepIndex);

    if (lastStep >= steps.size())
    {
        setLastStep(steps.size() - 1);
    }
}

void Song::insertStep(int stepIndex)
{
    steps.insert(steps.begin() + stepIndex, std::make_shared<Step>());
}

std::weak_ptr<Step> Song::getStep(int i)
{
    return steps[i];
}

int Song::getStepCount() const
{
    return steps.size();
}

bool Song::isUsed() const
{
    return used;
}

void Song::setUsed(bool b)
{
    used = b;

    if (!used)
    {
        name = "";
        steps.clear();
    }
}
