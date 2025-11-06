#pragma once

#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::file::aps
{
    class ApsGlobalParameters
    {
    private:
        bool padToInternalSound;
        bool padAssignMaster;
        bool stereoMixSourceDrum;
        bool indivFxSourceDrum;
        bool copyPgmMixToDrum;
        bool recordMixChanges;
        int masterLevel;
        int fxDrum;

    private:
        static std::vector<char> TEMPLATE_NO_SOUNDS;
        static std::vector<char> TEMPLATE_SOUNDS;

    public:
        std::vector<char> saveBytes{};

    private:
        int readFxDrum(char b) const;

    public:
        int getFxDrum() const;
        bool isPadToIntSoundEnabled() const;
        bool isPadAssignMaster() const;
        bool isStereoMixSourceDrum() const;
        bool isIndivFxSourceDrum() const;
        bool isCopyPgmMixToDrumEnabled() const;
        bool isRecordMixChangesEnabled() const;
        int getMasterLevel() const;

    public:
        std::vector<char> getBytes();

    public:
        ApsGlobalParameters(const std::vector<char> &loadBytes);
        ApsGlobalParameters(mpc::Mpc &mpc);
    };
} // namespace mpc::file::aps
