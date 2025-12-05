#include "sequencer/Song.hpp"

#include "sequencer/Step.hpp"

#include <algorithm>

using namespace mpc::sequencer;

void Song::setLoopEnabled(const bool b)
{
    loopEnabled = b;
}

bool Song::isLoopEnabled() const
{
    return loopEnabled;
}

void Song::setFirstLoopStepIndex(const SongStepIndex i)
{
    const auto max =
        std::max(MinSongStepIndex, SongStepIndex(getStepCount() - 1));

    firstLoopStepIndex = std::clamp(i, MinSongStepIndex, max);

    if (firstLoopStepIndex > lastLoopStepIndex)
    {
        setLastLoopStepIndex(firstLoopStepIndex);
    }
}

mpc::SongStepIndex Song::getFirstLoopStepIndex() const
{
    return firstLoopStepIndex;
}

void Song::setLastLoopStepIndex(const SongStepIndex i)
{
    const auto max =
        std::max(MinSongStepIndex, SongStepIndex(getStepCount() - 1));

    lastLoopStepIndex = std::clamp(i, MinSongStepIndex, max);

    if (lastLoopStepIndex < firstLoopStepIndex)
    {
        setFirstLoopStepIndex(lastLoopStepIndex);
    }
}

mpc::SongStepIndex Song::getLastLoopStepIndex() const
{
    return lastLoopStepIndex;
}

void Song::setName(const std::string &nameToUse)
{
    name = nameToUse;
}

std::string Song::getName()
{
    if (!used)
    {
        return "(Unused)";
    }

    return name;
}

void Song::deleteStep(const SongStepIndex stepIndex)
{
    if (stepIndex >= getStepCount())
    {
        return;
    }

    steps.erase(steps.begin() + stepIndex);

    if (lastLoopStepIndex >= getStepCount())
    {
        setLastLoopStepIndex(SongStepIndex(getStepCount() - 1));
    }
}

void Song::insertStep(const SongStepIndex stepIndex)
{
    steps.insert(steps.begin() + stepIndex, std::make_shared<Step>());
}

std::weak_ptr<Step> Song::getStep(const SongStepIndex i)
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

void Song::setUsed(const bool b)
{
    used = b;

    if (!used)
    {
        name = "";
        steps.clear();
    }
}
