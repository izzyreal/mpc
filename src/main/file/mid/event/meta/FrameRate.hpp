#pragma once

#include <string>
#include <vector>

namespace mpc::file::mid::event::meta
{
    class FrameRate
    {
    private:
        std::string name;
        int ordinal{-1};

    public:
        static FrameRate *FRAME_RATE_24;
        static FrameRate *FRAME_RATE_25;
        static FrameRate *FRAME_RATE_30_DROP;
        static FrameRate *FRAME_RATE_30;

    public:
        int value{};

    public:
        static FrameRate *fromInt(int val);

    private:
        FrameRate(std::string name, int ordinal, int v);

    public:
        static FrameRate *valueOf(std::string a0);
        static std::vector<FrameRate *> values();
    };
} // namespace mpc::file::mid::event::meta
