#pragma once

#include "performance/Drum.hpp"

#include <vector>

namespace mpc::engine
{
    class StereoMixer;
    class IndivFxMixer;
} // namespace mpc::engine

namespace mpc::file::aps
{
    class ApsMixer
    {

    public:
        std::vector<int> fxPaths = std::vector<int>(64);
        std::vector<int> levels = std::vector<int>(64);
        std::vector<int> pannings = std::vector<int>(64);
        std::vector<int> iLevels = std::vector<int>(64);
        std::vector<int> iOutputs = std::vector<int>(64);
        std::vector<int> sendLevels = std::vector<int>(64);
        std::vector<char> saveBytes = std::vector<char>(384);

        performance::StereoMixer getStereoMixerChannel(int noteIndex) const;
        performance::IndivFxMixer getIndivFxMixerChannel(int noteIndex) const;
        int getFxPath(int noteIndex) const;
        int getLevel(int noteIndex) const;
        int getPanning(int noteIndex) const;
        int getIndividualLevel(int noteIndex) const;
        int getIndividualOutput(int noteIndex) const;
        int getSendLevel(int noteIndex) const;
        std::vector<char> getBytes();

        ApsMixer(const std::vector<char> &loadBytes);
        ApsMixer(const std::vector<std::shared_ptr<engine::StereoMixer>>
                     &stereoMixer,
                 const std::vector<std::shared_ptr<engine::IndivFxMixer>>
                     &indivFxMixer);
    };
} // namespace mpc::file::aps
