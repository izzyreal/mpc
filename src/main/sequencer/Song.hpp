#pragma once

#include "IntTypes.hpp"

#include <vector>
#include <memory>
#include <string>

namespace mpc::sequencer
{
    class Step;
}

namespace mpc::sequencer
{
    class Song
    {
        std::string name;
        std::vector<std::shared_ptr<Step>> steps;
        bool loopEnabled = false;
        SongStepIndex firstLoopStepIndex{MinSongStepIndex};
        SongStepIndex lastLoopStepIndex{MinSongStepIndex};
        bool used = false;

    public:
        void setLoopEnabled(bool b);
        bool isLoopEnabled() const;
        void setFirstLoopStepIndex(SongStepIndex);
        SongStepIndex getFirstLoopStepIndex() const;
        void setLastLoopStepIndex(SongStepIndex);
        SongStepIndex getLastLoopStepIndex() const;
        void setName(const std::string &nameToUse);
        std::string getName();
        void deleteStep(SongStepIndex);
        void insertStep(SongStepIndex);

        std::weak_ptr<Step> getStep(SongStepIndex);
        int getStepCount() const;
        bool isUsed() const;
        void setUsed(bool b);
    };
} // namespace mpc::sequencer
