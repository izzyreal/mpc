#pragma once

#include <vector>

namespace mpc::file::aps
{
    class ApsDrumConfiguration
    {

    public:
        int programIndex;
        bool receivePgmChangeEnabled;
        bool receiveMidiVolumeEnabled;
        std::vector<char> saveBytes;

    private:
        static std::vector<char> TEMPLATE;
        static std::vector<char> PADDING;

    public:
        int getProgramIndex() const;
        bool isReceivePgmChangeEnabled() const;
        bool isReceiveMidiVolumeEnabled() const;

        std::vector<char> getBytes();

        ApsDrumConfiguration(const std::vector<char> &loadBytes);
        ApsDrumConfiguration(int program, bool recPgmChange,
                             bool recMidiVolume);
    };
} // namespace mpc::file::aps
