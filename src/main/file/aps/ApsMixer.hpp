#pragma once

#include <vector>
#include <memory>

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

    public:
        mpc::engine::StereoMixer getStereoMixerChannel(int noteIndex);
        mpc::engine::IndivFxMixer getIndivFxMixerChannel(int noteIndex);
        int getFxPath(int noteIndex) const;
        int getLevel(int noteIndex) const;
        int getPanning(int noteIndex) const;
        int getIndividualLevel(int noteIndex) const;
        int getIndividualOutput(int noteIndex) const;
        int getSendLevel(int noteIndex) const;
        std::vector<char> getBytes();

    public:
        ApsMixer(const std::vector<char> &loadBytes);
        ApsMixer(std::vector<std::shared_ptr<mpc::engine::StereoMixer>> &smc,
                 std::vector<std::shared_ptr<mpc::engine::IndivFxMixer>> &ifmc);
    };
} // namespace mpc::file::aps
