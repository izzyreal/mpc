#pragma once

#include <vector>
#include <string>

namespace mpc::sequencer
{
    class Sequence;
}

namespace mpc::file::all
{
    class Tracks
    {

        static const int TRACK_NAMES_OFFSET{0};
        static const int DEVICES_OFFSET{1024};
        static const int BUSSES_OFFSET{1088};
        static const int PGMS_OFFSET{1152};
        static const int VELO_RATIOS_OFFSET{1216};
        static const int STATUS_OFFSET{1280};
        static const int PADDING1_OFFSET{1408};
        static std::vector<char> PADDING1;
        static const int LAST_TICK_BYTE1_OFFSET{1424};
        static const int LAST_TICK_BYTE2_OFFSET{1425};
        static const int LAST_TICK_BYTE3_OFFSET{1426};
        static const int UNKNOWN32_BIT_INT_OFFSET{1428};
        std::vector<int> busses = std::vector<int>(64);
        std::vector<unsigned char> devices = std::vector<unsigned char>(64);
        std::vector<int> veloRatios = std::vector<int>(64);
        std::vector<int> pgms = std::vector<int>(64);
        std::vector<std::string> names = std::vector<std::string>(64);
        std::vector<bool> usednesses = std::vector<bool>(64);
        std::vector<bool> ons = std::vector<bool>(64);
        std::vector<bool> transmitProgramChangesEnableds =
            std::vector<bool>(64);

    public:
        std::vector<char> saveBytes{};

        int getDevice(int i) const;
        int getBus(int i) const;
        int getVelo(int i) const;
        int getPgm(int i) const;
        std::string getName(int i);
        bool isUsed(int i) const;
        bool isOn(int i) const;
        bool isTransmitProgramChangesEnabled(int i);

        std::vector<char> &getBytes();

        explicit Tracks(const std::vector<char> &loadBytes);
        explicit Tracks(sequencer::Sequence *seq);
    };
} // namespace mpc::file::all
