#pragma once

#include <vector>

namespace mpc::file::aps
{
    class ApsDrumConfiguration
    {

    public:
        int program;
        bool receivePgmChange;
        bool receiveMidiVolume;
        std::vector<char> saveBytes;

    private:
        static std::vector<char> TEMPLATE;
        static std::vector<char> PADDING;

    public:
        int getProgram() const;
        bool getReceivePgmChange() const;
        bool getReceiveMidiVolume() const;

        std::vector<char> getBytes();

        ApsDrumConfiguration(const std::vector<char> &loadBytes);
        ApsDrumConfiguration(int program, bool recPgmChange,
                             bool recMidiVolume);
    };
} // namespace mpc::file::aps
