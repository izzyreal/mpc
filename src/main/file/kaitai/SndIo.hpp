#pragma once

#include "mpc_types.hpp"

#include <memory>
#include <string>
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
    class SndIo
    {
    public:
        static sound_or_error loadBytes(
            const std::vector<char> &,
            const std::shared_ptr<sampler::Sound> &,
            const std::string &nameWithoutExtension);

        static sound_or_error loadSound(
            const std::shared_ptr<disk::MpcFile> &,
            const std::shared_ptr<sampler::Sound> &,
            const std::string &nameWithoutExtension);

        static std::vector<char> saveSound(sampler::Sound &);
    };
}
