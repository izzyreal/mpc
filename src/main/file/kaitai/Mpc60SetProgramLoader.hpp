#pragma once

#include "IntTypes.hpp"

#include <array>
#include <memory>

namespace mpc
{
    class Mpc;
}

namespace mpc::disk
{
    class MpcFile;
}

namespace mpc::file::kaitai
{
    struct Mpc60SetPreview;

    class Mpc60SetProgramLoader
    {
    public:
        static constexpr size_t kMpc60PadCount = 34;
        using ConversionTable = std::array<DrumNoteNumber, kMpc60PadCount>;

        static ConversionTable defaultConversionTable();

        static bool load(
            Mpc &mpc,
            const std::shared_ptr<disk::MpcFile> &file,
            const Mpc60SetPreview &preview,
            const ConversionTable &conversionTable,
            bool clearExisting);
    };
}
