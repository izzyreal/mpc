#include "sequencer/Song.hpp"

#include "SongStateView.hpp"

#include <algorithm>

using namespace mpc::sequencer;

Song::Song(const SongIndex songIndex,
           const std::function<SongStateView()> &getSnapshot,
           const std::function<void(SongMessage &&)> &dispatch)
    : songIndex(songIndex), getSnapshot(getSnapshot), dispatch(dispatch)
{
    name.reserve(Mpc2000XlSpecs::MAX_SONG_NAME_LENGTH);
}

void Song::setLoopEnabled(const bool b) const
{
    dispatch(SetSongLoopEnabled{songIndex, b});
}

bool Song::isLoopEnabled() const
{
    return getSnapshot().isLoopEnabled();
}

void Song::setFirstLoopStepIndex(const SongStepIndex i) const
{
    const auto max =
        std::max(MinSongStepIndex, SongStepIndex(getStepCount() - 1));

    dispatch(SetSongFirstLoopStepIndex{songIndex,
                                       std::clamp(i, MinSongStepIndex, max)});
}

mpc::SongStepIndex Song::getFirstLoopStepIndex() const
{
    return getSnapshot().getFirstLoopStepIndex();
}

void Song::setLastLoopStepIndex(const SongStepIndex i) const
{
    const auto max =
        std::max(MinSongStepIndex, SongStepIndex(getStepCount() - 1));

    dispatch(SetSongLastLoopStepIndex{songIndex,
                                      std::clamp(i, MinSongStepIndex, max)});
}

mpc::SongStepIndex Song::getLastLoopStepIndex() const
{
    return getSnapshot().getLastLoopStepIndex();
}

void Song::setName(const std::string &nameToUse)
{
    name = nameToUse;
}

std::string Song::getName()
{
    if (!getSnapshot().isUsed())
    {
        return "(Unused)";
    }

    return name;
}

void Song::deleteStep(const SongStepIndex stepIndex) const
{
    if (stepIndex >= getStepCount())
    {
        return;
    }

    dispatch(DeleteSongStep{songIndex, stepIndex});
}

void Song::insertStep(const SongStepIndex stepIndex) const
{
    dispatch(InsertSongStep{songIndex, stepIndex});
}

void Song::setStepSequenceIndex(const SongStepIndex stepIndex,
                                const SequenceIndex sequenceIndex) const
{
    dispatch(SetSongStepSequenceIndex{songIndex, stepIndex, sequenceIndex});
}

void Song::setStepRepetitionCount(const SongStepIndex stepIndex,
                                  const int8_t reptitionCount) const
{
    dispatch(SetSongStepRepetitionCount{
        songIndex, stepIndex,
        std::clamp(reptitionCount, int8_t{0},
                   Mpc2000XlSpecs::MAX_SONG_STEP_REPETITION_COUNT)});
}

SongStepState Song::getStep(const SongStepIndex i) const
{
    return getSnapshot().getStep(i);
}

uint8_t Song::getStepCount() const
{
    return getSnapshot().getStepCount();
}

bool Song::isUsed() const
{
    return getSnapshot().isUsed();
}

void Song::setUsed(const bool b)
{
    dispatch(SetSongUsed{songIndex, b});

    if (!b)
    {
        name.clear();
        dispatch(DeleteSong{songIndex});
    }
}
