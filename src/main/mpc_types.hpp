#pragma once

#include <tl/expected.hpp>

#include <memory>

namespace mpc::sampler { class Sound; }

namespace mpc::disk {

class MpcFile;

struct mpc_io_error {
    std::string log_msg;
    std::string popup_msg;
};

}

using file_or_error  = tl::expected<std::shared_ptr<mpc::disk::MpcFile>, mpc::disk::mpc_io_error>;
using sound_or_error  = tl::expected<std::shared_ptr<mpc::sampler::Sound>, mpc::disk::mpc_io_error>;
using file_operation = std::function<file_or_error()>;
