#pragma once

#include "sequencer/SequenceMetaInfo.hpp"

#include <tl/expected.hpp>

#include <memory>
#include <vector>
#include <string>
#include <cstdint>

namespace mpc::math
{
    constexpr double pi = 3.14159265358979323846;
}

namespace mpc::input::midi
{
    struct MidiControlPresetV3;
}

namespace mpc::sampler
{
    class Sound;
    class Program;
} // namespace mpc::sampler
namespace mpc::sequencer
{
    class Sequence;
}
namespace mpc::file::wav
{
    class WavFile;
}

namespace mpc::disk
{

    class MpcFile;

    using mpc_io_error_msg = std::string;

} // namespace mpc::disk

using file_or_error = tl::expected<std::shared_ptr<mpc::disk::MpcFile>,
                                   mpc::disk::mpc_io_error_msg>;
using sound_or_error = tl::expected<std::shared_ptr<mpc::sampler::Sound>,
                                    mpc::disk::mpc_io_error_msg>;
using program_or_error = tl::expected<std::shared_ptr<mpc::sampler::Program>,
                                      mpc::disk::mpc_io_error_msg>;
using sequence_or_error =
    tl::expected<std::shared_ptr<mpc::sequencer::Sequence>,
                 mpc::disk::mpc_io_error_msg>;

using sequences_or_error =
    tl::expected<std::vector<std::shared_ptr<mpc::sequencer::Sequence>>,
                 mpc::disk::mpc_io_error_msg>;

using sequence_meta_infos_or_error =
    tl::expected<std::vector<mpc::sequencer::SequenceMetaInfo>,
                 mpc::disk::mpc_io_error_msg>;

using preset_or_error =
    tl::expected<std::shared_ptr<mpc::input::midi::MidiControlPresetV3>,
                 mpc::disk::mpc_io_error_msg>;

using wav_or_error = tl::expected<std::shared_ptr<mpc::file::wav::WavFile>,
                                  mpc::disk::mpc_io_error_msg>;

using LcdPixel = std::pair<uint8_t, uint8_t>;
using LcdLine = std::vector<LcdPixel>;
using LcdBitmap = std::vector<LcdLine>;
