#pragma once

#include "IntTypes.hpp"
#include "SongState.hpp"

#include "sequencer/SongMessage.hpp"

#include <string>
#include <functional>

namespace mpc::sequencer
{
    class Step;
    class SongStateView;
    class Song final
    {
    public:
        explicit Song(SongIndex,
                      const std::function<SongStateView()> &getSnapshot,
                      const std::function<void(SongMessage &&)> &dispatch);

        void setLoopEnabled(bool b) const;
        bool isLoopEnabled() const;
        void setFirstLoopStepIndex(SongStepIndex) const;
        SongStepIndex getFirstLoopStepIndex() const;
        void setLastLoopStepIndex(SongStepIndex) const;
        SongStepIndex getLastLoopStepIndex() const;
        void setName(const std::string &nameToUse);
        std::string getName();
        void deleteStep(SongStepIndex) const;
        void insertStep(SongStepIndex) const;

        void setStepSequenceIndex(SongStepIndex, SequenceIndex) const;

        void setStepRepetitionCount(SongStepIndex, int8_t reptitionCount) const;

        SongStepState getStep(SongStepIndex) const;
        uint8_t getStepCount() const;
        bool isUsed() const;
        void setUsed(bool b);

    private:
        const SongIndex songIndex;
        std::string name;
        const std::function<SongStateView()> getSnapshot;
        const std::function<void(SongMessage &&)> dispatch;
    };
} // namespace mpc::sequencer
