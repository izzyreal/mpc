#pragma once

#include "mpc_types.hpp"

#include <cstddef>
#include <memory>
#include <vector>

namespace mpc::disk
{
    class MpcFile;
}

namespace mpc::sampler
{
    class Sound;
}

namespace mpc::file::kaitai
{
    struct Mpc60SetPreview;

    class Mpc60SetSoundLoader
    {
    public:
        static sound_or_error loadSoundDirectoryEntry(
            const std::shared_ptr<disk::MpcFile> &file,
            size_t soundDirectoryEntryIndex,
            const std::shared_ptr<sampler::Sound> &sound);

        static sound_or_error loadSoundDirectoryEntry(
            const std::vector<char> &bytes,
            size_t soundDirectoryEntryIndex,
            const std::shared_ptr<sampler::Sound> &sound);

        static sound_or_error
        loadSoundDirectoryEntry(const Mpc60SetPreview &preview,
                                size_t soundDirectoryEntryIndex,
                                const std::shared_ptr<sampler::Sound> &sound);

        static sound_or_error loadAssignedSoundAtMpc60Pad(
            const std::shared_ptr<disk::MpcFile> &file,
            size_t mpc60PadIndex,
            const std::shared_ptr<sampler::Sound> &sound);

        static sound_or_error loadAssignedSoundAtMpc60Pad(
            const std::vector<char> &bytes,
            size_t mpc60PadIndex,
            const std::shared_ptr<sampler::Sound> &sound);

        static sound_or_error loadAssignedSoundAtMpc60Pad(
            const Mpc60SetPreview &preview, size_t mpc60PadIndex,
            const std::shared_ptr<sampler::Sound> &sound);
    };
}
