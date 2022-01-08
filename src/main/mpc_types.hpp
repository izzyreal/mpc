#pragma once

#include <tl/expected.hpp>

#include <memory>
#include <vector>

namespace mpc::sampler { class Sound; class Program; }
namespace mpc::sequencer { class Sequence; }
namespace mpc::file::wav { class WavFile; }

namespace mpc::disk {

class MpcFile;

struct mpc_io_error {
    std::string log_msg;
    std::string popup_msg;
};

}

using file_or_error      = tl::expected<std::shared_ptr<mpc::disk::MpcFile>, mpc::disk::mpc_io_error>;
using sound_or_error     = tl::expected<std::shared_ptr<mpc::sampler::Sound>, mpc::disk::mpc_io_error>;
using program_or_error   = tl::expected<std::shared_ptr<mpc::sampler::Program>, mpc::disk::mpc_io_error>;
using sequence_or_error  = tl::expected<std::shared_ptr<mpc::sequencer::Sequence>, mpc::disk::mpc_io_error>;
using sequences_or_error = tl::expected<std::vector<std::shared_ptr<mpc::sequencer::Sequence>>, mpc::disk::mpc_io_error>;
using void_or_error      = tl::expected<void, mpc::disk::mpc_io_error>;
using wav_or_error       = tl::expected<mpc::file::wav::WavFile, mpc::disk::mpc_io_error>;
