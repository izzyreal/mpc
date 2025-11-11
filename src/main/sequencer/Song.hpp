#pragma once

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
        int firstStep = 0;
        int lastStep = 0;
        bool used = false;

    public:
        void setLoopEnabled(bool b);
        bool isLoopEnabled() const;
        void setFirstStep(int i);
        int getFirstStep() const;
        void setLastStep(int i);
        int getLastStep() const;
        void setName(const std::string &string);
        std::string getName();
        void deleteStep(int stepIndex);
        void insertStep(int stepIndex);

        std::weak_ptr<Step> getStep(int i);
        int getStepCount() const;
        bool isUsed() const;
        void setUsed(bool b);
    };
} // namespace mpc::sequencer
